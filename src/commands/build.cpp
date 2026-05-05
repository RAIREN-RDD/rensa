#include "rensa/commands.hpp"
#include "rensa/core.hpp"
#include "rensa/log.hpp"

#include <filesystem>
#include <fstream>
#include <print>
#include <sstream>
#include <string>

namespace rairen::rensa {

static Path find_build_file(Path start) {
  start = fs::absolute(start);

  while (true) {
    Path candidate = start / "build.rensa";

    if (fs::exists(candidate) && fs::is_regular_file(candidate)) {
      return candidate;
    }

    if (!start.has_parent_path() || start == start.root_path()) {
      break;
    }

    start = start.parent_path();
  }

  return {};
}

RENSA_COMMAND(build) {
  Path start_dir = ".";

  if (!args.empty()) {
    start_dir = args[0];
  }

  if (!fs::exists(start_dir)) {
    log(LogLevel::Error, "Path '{}' does not exist", start_dir.string());
    return SystemStatus::Error;
  }

  if (!fs::is_directory(start_dir)) {
    log(LogLevel::Error, "Path '{}' is not a directory", start_dir.string());
    return SystemStatus::Error;
  }

  Path build_file = find_build_file(start_dir);

  if (build_file.empty()) {
    log(LogLevel::Error, "No build.rensa found starting from '{}'",
        start_dir.string());
    return SystemStatus::Error;
  }

  build_file = fs::weakly_canonical(build_file);

  std::ifstream f(build_file);

  using TargetName = String;
  using VarName = String;

  StringView project_name = "";
  StringView project_version = "";
  Map<VarName, String> global_vars;
  Map<TargetName, Map<VarName, String>> targets_vars;
  Vector<String> targets;

  String line;
  Vector<String> tokens;

  bool inside_target = false;
  bool inside_global_vars = false;
  bool inside_target_vars = false;

  usize line_number = 0;
  while (std::getline(f, line)) {
    line_number++;

    if (line.empty())
      continue;

    std::stringstream iss(line);

    tokens.clear();
    String token;

    while (iss >> token) {
      tokens.push_back(token);
    }

    StringView directive = tokens[0];

    if (directive == "PROJECT_NAME") {
      if (tokens.size() < 2) {
        log(LogLevel::Error, "line {}: Project name missing", line_number);
        return SystemStatus::Error;
      }
      project_name = tokens[1];
    }

    if (directive == "PROJECT_VERSION") {
      if (tokens.size() < 2) {
        log(LogLevel::Error, "line {}: Project version missing", line_number);
        return SystemStatus::Error;
      }
      project_version = tokens[1];
    }

    if (directive == "TARGET") {
      if (tokens.size() < 2) {
        log(LogLevel::Error, "line {}: Target name  missing", line_number);
        return SystemStatus::Error;
      }

      inside_target = true;
      targets.push_back(tokens[1]);
    }

    if (directive == "VARS") {
      if (inside_target) {
        inside_target_vars = true;
      } else {
        inside_global_vars = true;
      }
    }

    

    if (directive == "END") {
      if (inside_target) {
        inside_target = false;
        continue;
      }

      if (inside_global_vars) {
        inside_global_vars = false;
        continue;
      }

      if (inside_target_vars) {
        inside_target = false;
        continue;
      }

      log(LogLevel::Error, "line {}: Unnecessary END directive", line_number);
      return SystemStatus::Error;
    }
    std::println("{}", directive);
  }

  return SystemStatus::Success;
}

} // namespace rairen::rensa