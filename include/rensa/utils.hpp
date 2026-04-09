#pragma once // utils.hpp

#include <array>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <functional>
#include <print>
#include <string>
#include <string_view>
#include <vector>

#include "rensa/ext/yaml-cpp/yaml.h"

namespace rensa {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using String = std::string;
using StringView = std::string_view;

template <typename _T> using Vector = std::vector<_T>;
template <typename _T, std::size_t _N> using Array = std::array<_T, _N>;

namespace fs = std::filesystem;
using Path = fs::path;

using Yaml = YAML::Node;

// If condition is true, print `msg` and return `true`, else return `false`
template <typename... Args>
[[nodiscard]] bool check_if(bool condition, std::format_string<Args...> msg,
                            Args &&...args) {
  if (condition) {
    std::println(msg, std::forward<Args>(args)...);
    return true;
  }
  return false;
}

// List files recursively inside `path` filtered by `filter`. If no filter is
// passed, return all files found.
std::expected<void, int> list_files_recursively(
    StringView path, Vector<String> &paths,
    std::move_only_function<bool(const fs::path &)> filter = nullptr);

enum class LogLevel { INFO, WARNING, ERROR };

// Simple helper to print level as string
inline constexpr std::string_view log_level_str(LogLevel lvl) {
  switch (lvl) {
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARNING:
    return "WARNING";
  case LogLevel::ERROR:
    return "ERROR";
  }
  return "UNKNOWN";
}

template <typename... Args>
inline void log(LogLevel lvl, std::format_string<Args...> msg, Args &&...args) {
  std::println("rensa: [{}] {}", log_level_str(lvl),
               std::format(msg, std::forward<Args>(args)...));
}

} // namespace rensa

using rensa::i32;
