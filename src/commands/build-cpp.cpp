#include "rensa/commands.hpp"
#include "rensa/core.hpp"
#include "rensa/log.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include <openssl/evp.h>

namespace rairen::rensa {

static int run_process(const std::vector<std::string> &args) {
  std::vector<char *> argv;
  argv.reserve(args.size() + 1);

  for (const auto &s : args)
    argv.push_back(const_cast<char *>(s.c_str()));

  argv.push_back(nullptr);

  pid_t pid = fork();

  if (pid == 0) {
    execvp(argv[0], argv.data());
    _exit(127);
  }

  if (pid < 0) {
    return -1;
  }

  int status = 0;
  waitpid(pid, &status, 0);

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }

  return -1;
}

static bool is_watched_extension(const Path &p, const StringView exts) {
  if (!p.has_extension())
    return false;

  auto ext = p.extension().string();
  if (!ext.empty() && ext[0] == '.') {
    ext.erase(ext.begin());
  }

  return exts.contains(ext);
}

void list_files_recursively(const Path &root, Vector<Path> &out,
                            const StringView exts) {
  std::error_code ec;

  fs::recursive_directory_iterator it(
      root, fs::directory_options::skip_permission_denied, ec);

  fs::recursive_directory_iterator end;

  if (ec)
    return;

  for (; it != end; it.increment(ec)) {
    if (ec) {
      ec.clear();
      continue;
    }

    const auto &entry = *it;

    if (!entry.is_regular_file(ec)) {
      continue;
    }

    if (ec) {
      ec.clear();
      continue;
    }

    if (!is_watched_extension(entry.path(), exts)) {
      continue;
    }

    out.push_back(entry.path());
  }
}

std::string sha2_256_file(const Path &path) {
  constexpr size_t buffer_size = 1 << 15;

  std::ifstream file(path, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Cannot open file: " + path.string());
  }

  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  if (!ctx) {
    throw std::runtime_error("Failed to create EVP context");
  }

  if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
    EVP_MD_CTX_free(ctx);
    throw std::runtime_error("DigestInit failed");
  }

  std::vector<char> buffer(buffer_size);

  while (file) {
    file.read(buffer.data(), buffer.size());
    std::streamsize bytes = file.gcount();

    if (bytes > 0) {
      if (EVP_DigestUpdate(ctx, buffer.data(), static_cast<size_t>(bytes)) !=
          1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("DigestUpdate failed");
      }
    }
  }

  unsigned char hash[EVP_MAX_MD_SIZE];
  unsigned int hash_len = 0;

  if (EVP_DigestFinal_ex(ctx, hash, &hash_len) != 1) {
    EVP_MD_CTX_free(ctx);
    throw std::runtime_error("DigestFinal failed");
  }

  EVP_MD_CTX_free(ctx);

  std::ostringstream oss;
  oss << std::hex << std::setfill('0');

  for (unsigned int i = 0; i < hash_len; ++i) {
    oss << std::setw(2) << static_cast<int>(hash[i]);
  }

  return oss.str();
}

RENSA_COMMAND(build_cpp) {
  Vector<Path> watched_headers;
  Vector<Path> watched_sources;

  Vector<Path> folders_to_watch = {"src", "include"};
  Vector<String> cxx_flags = {"-std=c++23", "-Wall", "-Iinclude"};

  Path build_dir = "build";
  String compiler = "g++";
  Path output_binary = build_dir / "app";

  fs::create_directories(build_dir);

  for (const auto &folder : folders_to_watch) {
    list_files_recursively(folder, watched_headers, "hpp");
    list_files_recursively(folder, watched_sources, "cpp");
  }

  fs::create_directories(orchestrator->rensa_dir);
  std::ofstream f(orchestrator->rensa_cache_path);

  for (const auto &file : watched_headers) {
    f << std::format("{} | {}\n", file.string(), sha2_256_file(file));
  }

  f << "-\n";

  for (const auto &file : watched_sources) {
    f << std::format("{} | {}\n", file.string(), sha2_256_file(file));
  }

  f.close();

  Vector<Path> objects;

  for (const auto &src : watched_sources) {
    Path obj = build_dir / (src.filename().string() + ".o");
    objects.push_back(obj);

    std::vector<std::string> cmd = {compiler, "-c", src.string(), "-o",
                                    obj.string()};

    for (const auto &flag : cxx_flags)
      cmd.push_back(flag);

    int rc = run_process(cmd);

    if (rc != 0) {
      std::cerr << "Compilation failed: " << src << "\n";
      return SystemStatus::Error;
    }
  }

  std::vector<std::string> link_cmd = {compiler};

  for (const auto &obj : objects)
    link_cmd.push_back(obj.string());

  link_cmd.push_back("-o");
  link_cmd.push_back(output_binary.string());

  int rc = run_process(link_cmd);

  if (rc != 0) {
    std::cerr << "Linking failed\n";
    return SystemStatus::Error;
  }

  std::cout << "Build successful: " << output_binary << "\n";
  return SystemStatus::Success;
}
} // namespace rairen::rensa
