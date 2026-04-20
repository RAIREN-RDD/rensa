#include "rensa/commands.hpp"
#include "rensa/log.hpp"

#include <cctype>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include <archive.h>
#include <archive_entry.h>
#include <lzma.h>

namespace rairen::rensa {

bool is_probably_text(const uint8_t *data, size_t size) {
  size_t sample = std::min<size_t>(size, 512);

  for (size_t i = 0; i < sample; i++) {
    uint8_t c = data[i];

    if (c == 0)
      return false;

    if (c < 9 && c != '\n' && c != '\r')
      return false;

    if (c > 0x7E && c < 0xA0)
      return false;
  }

  return true;
}

std::string
replace_tokens(const std::string &input,
               const std::unordered_map<std::string, std::string> &vars) {

  std::string out;
  out.reserve(input.size());

  for (size_t i = 0; i < input.size();) {

    if (i + 7 < input.size() && input[i] == '_' && input[i + 1] == '_') {

      size_t end = input.find("__", i + 2);
      if (end != std::string::npos) {

        std::string key = input.substr(i + 2, end - (i + 2));

        auto it = vars.find(key);
        if (it != vars.end()) {
          out += it->second;
        } else {
          out += "__" + key + "__";
        }

        i = end + 2;
        continue;
      }
    }

    out.push_back(input[i]);
    i++;
  }

  return out;
}

void decompress_xz_to_buffer(const std::vector<uint8_t> &input,
                             std::vector<uint8_t> &output) {
  lzma_stream strm = LZMA_STREAM_INIT;

  if (lzma_stream_decoder(&strm, UINT64_MAX, 0) != LZMA_OK)
    throw std::runtime_error("lzma init failed");

  strm.next_in = input.data();
  strm.avail_in = input.size();

  uint8_t buffer[8192];

  while (true) {
    strm.next_out = buffer;
    strm.avail_out = sizeof(buffer);

    lzma_ret ret = lzma_code(&strm, LZMA_RUN);

    if (ret != LZMA_OK && ret != LZMA_STREAM_END) {
      lzma_end(&strm);
      throw std::runtime_error(std::string("xz decode failed (code ") +
                               std::to_string(ret) + ")");
    }

    size_t written = sizeof(buffer) - strm.avail_out;
    output.insert(output.end(), buffer, buffer + written);

    if (ret == LZMA_STREAM_END)
      break;
  }

  lzma_end(&strm);
}

struct MemReader {
  const uint8_t *data;
  size_t size;
  size_t offset;
};

static la_ssize_t read_callback(struct archive *, void *client_data,
                                const void **buffer) {
  auto *mem = static_cast<MemReader *>(client_data);

  if (mem->offset >= mem->size)
    return 0;

  size_t chunk = std::min<size_t>(8192, mem->size - mem->offset);

  *buffer = mem->data + mem->offset;
  mem->offset += chunk;

  return static_cast<la_ssize_t>(chunk);
}

static int close_callback(struct archive *, void *) { return ARCHIVE_OK; }

std::optional<Path> resolve_template(StringView template_name,
                                     const Path &templates_path) {
  for (const auto &entry : fs::recursive_directory_iterator(templates_path)) {
    if (!entry.is_regular_file())
      continue;

    const auto &path = entry.path();

    if (path.extension() != ".rsmk")
      continue;

    auto relative = fs::relative(path, templates_path).replace_extension();

    if (relative.generic_string() == template_name)
      return path;
  }

  return std::nullopt;
}

RENSA_COMMAND(create) {
  if (args.size() < 1) {
    log(LogLevel::Error, "Project name missing");
    return SystemStatus::Error;
  }

  if (args.size() < 2) {
    log(LogLevel::Error, "Template name/source missing");
    return SystemStatus::Error;
  }

  StringView project_name = args[0];
  StringView template_name = args[1];

  if (fs::exists(project_name)) {
    log(LogLevel::Error, "Folder '{}' already exists", project_name);
    return SystemStatus::Error;
  }

  if (template_name.starts_with("http://") ||
      template_name.starts_with("https://")) {
    log(LogLevel::Error, "Template download not supported yet");
    return SystemStatus::Error;
  }

  auto resolved = resolve_template(template_name, orchestrator->templates_path);

  if (!resolved) {
    log(LogLevel::Error, "Template '{}' not found", template_name);
    return SystemStatus::Error;
  }

  log(LogLevel::Note, "Creating project '{}' from template '{}'", project_name,
      template_name);

  fs::create_directory(project_name);

  std::unordered_map<std::string, std::string> vars = {
      {"RENSA_PROJECT_NAME", std::string(project_name)}};

  std::vector<uint8_t> compressed;
  {
    std::ifstream in(resolved->string(), std::ios::binary);
    if (!in)
      throw std::runtime_error("failed to open template file");

    compressed.assign(std::istreambuf_iterator<char>(in),
                      std::istreambuf_iterator<char>());
  }

  std::vector<uint8_t> tar_data;
  decompress_xz_to_buffer(compressed, tar_data);

  MemReader mem{tar_data.data(), tar_data.size(), 0};

  archive *a = archive_read_new();
  archive_read_support_format_tar(a);
  archive_read_support_filter_none(a);

  if (archive_read_open(a, &mem, nullptr, read_callback, close_callback) !=
      ARCHIVE_OK) {
    std::string err = archive_error_string(a);
    archive_read_free(a);
    throw std::runtime_error(err);
  }

  archive_entry *entry;

  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    const char *path = archive_entry_pathname(entry);

    fs::path outPath = fs::path(project_name) / path;

    if (archive_entry_filetype(entry) == AE_IFDIR) {
      fs::create_directories(outPath);
      continue;
    }

    fs::create_directories(outPath.parent_path());

    const void *buff;
    size_t size;
    la_int64_t offset;

    std::vector<uint8_t> file_data;

    while (true) {
      int r = archive_read_data_block(a, &buff, &size, &offset);

      if (r == ARCHIVE_EOF)
        break;

      if (r < ARCHIVE_OK) {
        std::string err = archive_error_string(a);
        archive_read_free(a);
        throw std::runtime_error(err);
      }

      const uint8_t *data = static_cast<const uint8_t *>(buff);
      file_data.insert(file_data.end(), data, data + size);
    }

    bool is_text = is_probably_text(file_data.data(), file_data.size());
    if (is_text) {
      std::string content(file_data.begin(), file_data.end());

      content = replace_tokens(content, vars);

      std::ofstream out(outPath);
      out << content;
    } else {
      std::ofstream out(outPath, std::ios::binary);

      out.write(reinterpret_cast<const char *>(file_data.data()),
                file_data.size());
    }
  }

  archive_read_close(a);
  archive_read_free(a);

  return SystemStatus::Success;
}

} // namespace rairen::rensa
