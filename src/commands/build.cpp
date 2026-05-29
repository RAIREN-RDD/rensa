#include "rensa/builders.hpp"
#include "rensa/commands.hpp"
#include "rensa/log.hpp"

#include <expected>
#include <fstream>

#include <print>
#include <yaml-cpp/yaml.h>

namespace rairen::rensa {

struct Target {
  Vars vars;
  Builder *builder = nullptr;
};

using File = OrderedMap<u64, Vector<String>>;
using Iter = File::iterator;

/* ----- VARS PARSER ----- */
std::expected<Vars, SystemStatus> create_vars(const File &file, Iter &it) {
  Vars vars;

  for (++it; it != file.end(); ++it) {
    auto &[line_number, tokens] = *it;

    if (tokens.empty()) {
      continue;
    }

    if (tokens[0] == "END") {
      return vars;
    }

    const String &key = tokens[0];

    // var decl
    if (tokens.size() == 1) {
      vars[key];
      continue;
    }

    // decl + op
    if (tokens.size() < 3) {
      rensa::log(LogLevel::Error,
                 "Invalid variable syntax on line {} (operator requires RHS)",
                 line_number);
      return std::unexpected(SystemStatus::Error);
    }

    const String &op = tokens[1];

    if (op != "=" && op != "?=" && op != "+=") {
      rensa::log(LogLevel::Error, "Unknown operator '{}' on line {}", op,
                 line_number);
      return std::unexpected(SystemStatus::Error);
    }

    auto rhs_begin = tokens.begin() + 2;

    // = const, ?= overwrite
    if (op == "=" || op == "?=") {
      vars[key] = Vector<String>(rhs_begin, tokens.end());
      continue;
    }

    // += append
    if (op == "+=") {
      auto &vec = vars[key];
      vec.insert(vec.end(), rhs_begin, tokens.end());
      continue;
    }
  }

  rensa::log(LogLevel::Error, "Directive 'END' missing in VARS block");
  return std::unexpected(SystemStatus::Error);
}

/* ----- TARGET PARSER ----- */
std::expected<Target, SystemStatus> create_target(const File &file, Iter &it) {
  Target target{};

  const auto &start_tokens = it->second;

  if (start_tokens.size() < 2) {
    rensa::log(LogLevel::Error, "Target name missing on line {}", it->first);
    return std::unexpected(SystemStatus::Error);
  }

  const String target_name = start_tokens[1];

  for (++it; it != file.end(); ++it) {
    auto &[line_number, tokens] = *it;

    if (tokens.empty()) {
      continue;
    }

    const String &op = tokens[0];

    if (op == "VARS") {
      auto vars_res = create_vars(file, it);
      if (!vars_res) {
        return std::unexpected(vars_res.error());
      }
      target.vars = std::move(*vars_res);
      target.vars["TARGET_NAME"] = {target_name};
      continue;
    }

    if (op == "SET_BUILDER") {
      if (tokens.size() < 2) {
        rensa::log(LogLevel::Error, "Builder name missing on line {}",
                   line_number);
        return std::unexpected(SystemStatus::Error);
      }

      const StringView builder = tokens[1];

      if (builder == "@rairen/rensa-cpp23") {
        if (target.builder) {
          delete target.builder;
        }
        target.builder = new RensaCpp23();
        continue;
      }

      rensa::log(LogLevel::Error, "Unknown builder '{}' on line {}", builder,
                 line_number);

      return std::unexpected(SystemStatus::Error);
    }

    if (op == "END") {
      if (!target.builder) {
        rensa::log(LogLevel::Error, "Builder not set for target '{}'",
                   target_name);
        return std::unexpected(SystemStatus::Error);
      }

      return target;
    }

    rensa::log(LogLevel::Error, "Unknown directive '{}' on line {}", op,
               line_number);

    return std::unexpected(SystemStatus::Error);
  }

  rensa::log(LogLevel::Error, "Unterminated TARGET block for '{}'",
             target_name);

  return std::unexpected(SystemStatus::Error);
}

/* ----- COMMAND BUILD ----- */
RENSA_COMMAND(build) {
  UnorderedMap<String, Target> targets;

  String target_name = args.size() > 1 ? args[1] : "";

  std::ifstream file_in("build.rensa");

  if (!file_in.is_open()) {
    rensa::log(LogLevel::Error, "Failed to open 'build.rensa'");
    return SystemStatus::Error;
  }

  File file;

  String line;
  String token;
  u64 line_number = 0;

  while (std::getline(file_in, line)) {
    ++line_number;

    if (line.empty())
      continue;

    std::stringstream ss(line);
    Vector<String> tokens;

    while (ss >> token) {
      if (token == "#")
        break;
      tokens.push_back(token);
    }

    if (!tokens.empty()) {
      file[line_number] = std::move(tokens);
    }
  }

  for (auto it = file.begin(); it != file.end(); ++it) {
    const auto &[line_index, tokens] = *it;

    if (tokens.empty())
      continue;

    if (tokens[0] == "TARGET" || tokens[0] == "TARGET_DEFAULT") {

      if (tokens.size() != 2) {
        rensa::log(LogLevel::Error, "Invalid target declaration on line '{}'",
                   line_index);
        return SystemStatus::Error;
      }

      const String &name = tokens[1];

      if (tokens[0] == "TARGET_DEFAULT")
        target_name = name;

      if (targets.contains(name)) {
        rensa::log(LogLevel::Error, "Duplicate target '{}' on line '{}'", name,
                   line_index);
        return SystemStatus::Error;
      }

      auto res = create_target(file, it);
      if (!res) {
        return res.error();
      }

      targets[name] = std::move(*res);
      continue;
    }

    if (tokens[0] == "END") {
      rensa::log(LogLevel::Error, "Unexpected END directive on line '{}'",
                 line_index);
      return SystemStatus::Error;
    }
  }

  if (!targets.contains(target_name)) {
    rensa::log(LogLevel::Error, "Target '{}' not found", target_name);
    return SystemStatus::Error;
  }

  Target &target = targets.at(target_name);
  SystemStatus out = target.builder->build(target.vars);

  for (auto &[_, t] : targets) {
    delete t.builder;
    t.builder = nullptr;
  }

  return out;
}

} // namespace rairen::rensa