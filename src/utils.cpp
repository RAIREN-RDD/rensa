#include "rensa/utils.hpp"

namespace rensa {

// List files recursively inside `path` filtered by `filter`. If no filter is
// passed, return all files found.
std::expected<void, int>
list_files_recursively(StringView path, Vector<String> &paths,
                       std::move_only_function<bool(const fs::path &)> filter) {
  fs::path dir_path{String(path)};

  if (!fs::exists(dir_path)) {
    return std::unexpected{1}; // path does not exist
  }

  if (!fs::is_directory(dir_path)) {
    return std::unexpected{2}; // path is not a directory
  }

  try {
    for (auto const &entry : fs::recursive_directory_iterator(dir_path)) {
      if (!entry.is_regular_file())
        continue;
      const fs::path &p = entry.path();
      if (!filter || filter(p)) {
        paths.push_back(p.string());
      }
    }
  } catch (const fs::filesystem_error &e) {
    return std::unexpected{3}; // filesystem error
  }

  return {}; // success
}

} // namespace rensa