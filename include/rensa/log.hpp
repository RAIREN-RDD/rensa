#include "rensa/types.hpp"

#include <format>
#include <print>
#include <unistd.h>

namespace rairen::rensa {

enum class LogLevel { Note, Warning, Error };

static bool supports_color() {
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE)
    return false;

  DWORD mode = 0;
  if (!GetConsoleMode(hOut, &mode))
    return false;

  return mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
#else
  const char *term = std::getenv("TERM");
  if (!term)
    return false;

  if (std::string(term) == "dumb")
    return false;

  return isatty(fileno(stdout));
#endif
}

static const char *color(LogLevel level) {
  switch (level) {
  case LogLevel::Note:
    return "\x1b[32m";
  case LogLevel::Warning:
    return "\x1b[33m";
  case LogLevel::Error:
    return "\x1b[31m";
  }
  return "";
}

static const char *reset() { return "\x1b[0m"; }

template <typename... Args>
void log(LogLevel level, std::format_string<Args...> fmt, Args &&...args) {
  std::string prefix = "rensa: [";

  if (supports_color()) {
    prefix += color(level);

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

    prefix += reset();
  } else {
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
  }

  prefix += "] ";

  std::println("{}{}", prefix, std::format(fmt, std::forward<Args>(args)...));
}

} // namespace rairen::rensa