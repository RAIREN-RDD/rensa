#pragma once

#include <cstdlib>
#include <format>
#include <print>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace rairen::rensa {

enum class LogLevel { Note, Warning, Error };

enum class Color { Reset, Green, Yellow, Red };

static bool g_colors_enabled = []() -> bool {
#ifdef _WIN32
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE)
    return false;

  DWORD mode = 0;
  if (!GetConsoleMode(hOut, &mode))
    return false;

  return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
#else
  const char *term = std::getenv("TERM");
  if (!term)
    return false;

  if (std::string(term) == "dumb")
    return false;

  return isatty(fileno(stdout)) != 0;
#endif
}();

static const char *to_ansi(Color c) {
  if (!g_colors_enabled)
    return "";

  switch (c) {
  case Color::Green:
    return "\x1b[32m";
  case Color::Yellow:
    return "\x1b[33m";
  case Color::Red:
    return "\x1b[31m";
  case Color::Reset:
    return "\x1b[0m";
  }
  return "";
}

static const char *level_name(LogLevel level) {
  switch (level) {
  case LogLevel::Note:
    return "NOTE";
  case LogLevel::Warning:
    return "WARNING";
  case LogLevel::Error:
    return "ERROR";
  }
  return "";
}

static Color level_color(LogLevel level) {
  switch (level) {
  case LogLevel::Note:
    return Color::Green;
  case LogLevel::Warning:
    return Color::Yellow;
  case LogLevel::Error:
    return Color::Red;
  }
  return Color::Reset;
}

template <typename... Args>
void log(LogLevel level, std::format_string<Args...> fmt, Args &&...args) {

  std::string prefix = "rensa: [";

  prefix += to_ansi(level_color(level));
  prefix += level_name(level);
  prefix += to_ansi(Color::Reset);

  prefix += "] ";

  std::println("{}{}", prefix, std::format(fmt, std::forward<Args>(args)...));
}

} // namespace rairen::rensa