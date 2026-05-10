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

  struct Statuses {
    bool inside_target = false;
    bool inside_var = false;
    bool inside_depends = false;
  };

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

  Statuses statuses;
  usize line_number = 0;

  Vector<String> known_directives = {
      "PROJECT_NAME", "PROJECT_VERSION", "VARS", "DEPENDS_ON", "END", "TARGET"};

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

    if (statuses.inside_var) {
      if (directive == "END") {
        statuses.inside_var = false;
        continue;
      }

      
      if (!statuses.inside_target) {
        std::println("GLOBAL {}", directive);
        global_vars[String(directive)] = "";
      } else {
        std::println("LOCAL {}", directive);
        targets_vars[targets.back()][String(directive)] = "";
      }

      continue;
    }

    if (statuses.inside_target) {
      if (directive == "END") {
        statuses.inside_target = false;
        continue;
      }
    }

    if (std::find(known_directives.begin(), known_directives.end(),
                  String(directive)) == known_directives.end()) {
      log(LogLevel::Error, "Unknown directive {} on line {}", directive,
          line_number);
      return SystemStatus::Error;
    }

    if (directive == "PROJECT_NAME") {
      if (tokens.size() < 2) {
        log(LogLevel::Error, "Project name missing on line {}", line_number);
        return SystemStatus::Error;
      }

      if (tokens.size() > 2) {
        log(LogLevel::Error, "Too many arguments for directive {} on line {}",
            directive, line_number);
        return SystemStatus::Error;
      }

      global_vars["PROJECT_NAME"] = tokens[1];
    }

    if (directive == "PROJECT_VERSION") {
      if (tokens.size() < 2) {
        log(LogLevel::Error, "Project version missing on line {}", line_number);
        return SystemStatus::Error;
      }

      if (tokens.size() > 2) {
        log(LogLevel::Error, "Too many arguments for directive {} on line {}",
            directive, line_number);
        return SystemStatus::Error;
      }

      global_vars["PROJECT_VERSION"] = tokens[1];
    }

    if (directive == "VARS") {
      if (tokens.size() > 1) {
        log(LogLevel::Error, "No arguments are expected on line {}",
            line_number);
        return SystemStatus::Error;
      }

      statuses.inside_var = true;
    }

    if (directive == "TARGET") {
      if (tokens.size() < 2) {
        log(LogLevel::Error, "Target name missing on line {}", line_number);
        return SystemStatus::Error;
      }

      if (tokens.size() > 2) {
        log(LogLevel::Error, "Too many arguments for directive {} on line {}",
            directive, line_number);
        return SystemStatus::Error;
      }

      statuses.inside_target = true;
      targets.push_back(tokens[1]);
    }
  }

  return SystemStatus::Success;
}

} // namespace rairen::rensa