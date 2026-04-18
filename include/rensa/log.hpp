#include "rensa/types.hpp"

#include <format>
#include <print>

namespace rairen::rensa {

enum class LogLevel { Note, Warning, Error };

template <typename... Args>
void log(LogLevel level, std::format_string<Args...> fmt, Args &&...args) {
  String prefix = "rensa: [";

  switch (level) {
  case LogLevel::Note:
    prefix += "NOTE";
    break;
  case LogLevel::Warning:
    prefix += "WARNING";
    break;
  case LogLevel::Error:
    prefix += "ERROR";
    break;
  }

  prefix += "] ";

  std::println("{}{}", prefix, std::format(fmt, std::forward<Args>(args)...));
}

} // namespace rairen::rensa