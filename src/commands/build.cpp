#include "rensa/commands.hpp"
#include "rensa/core.hpp"
#include "rensa/log.hpp"
#include "rensa/types.hpp"

#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <print>
#include <set>
#include <sstream>
#include <string>
#include <unordered_set>

#include <yaml-cpp/yaml.h>

namespace rairen::rensa {

using Vars = UnorderedMap<String, std::set<String>>;

#define try_assign(var, varname)                                               \
  if (!vars.contains(varname)) {                                               \
    rensa::log(LogLevel::Error, "Argument '{}' missing", varname);             \
    return SystemStatus::Error;                                                \
  }                                                                            \
  StringView var = vars[varname]

#define try_assign_opt(var, name)                                              \
  StringView var = (vars).contains(name) ? (vars)[name] : StringView {}

struct Builder {
  Vars vars;

  void init(Vars &vars) { this->vars = vars; }

  virtual ~Builder() = default;

  virtual SystemStatus build() = 0;
};

#undef try_assign

struct Target {
  Vars vars;
  Builder *builder = nullptr;
};

using File = OrderedMap<u64, Vector<String>>;
using Iter = File::iterator;

Vars create_vars(const File &file, Iter &it) {
  Vars vars;

  for (++it; it != file.end(); ++it) {
    auto &[line_number, tokens] = *it;

    if (tokens.empty()) {
      continue;
    }

    if (tokens[0] == "END") {
      return vars;
    }

    if (tokens.size() < 2) {
      rensa::log(LogLevel::Error, "Invalid variable declaration on line '{}'",
                 line_number);

      continue;
    }

    const String &key = tokens[0];

    String value;

    if (tokens.size() == 1) {
      vars[key] = {};
      continue;
    }

    if (tokens.size() == 2) {
      rensa::log(LogLevel::Error, "Missing value on line '{}'", line_number);

      continue;
    }

    String op = tokens[1];

    if (!std::set<String>({"=", "?=", "+="}).contains(op)) {
      rensa::log(LogLevel::Error, "Unknown operator '{}' on line {}", op,
                 line_number);

      continue;
    }

    if (op == "=" || op == "?=") {
      vars[key] = std::set<String>(tokens.begin() + 2, tokens.end());
      continue;
    }

    if (op == "+=") {
      vars[key].insert(tokens.begin() + 2, tokens.end());
      continue;
    }
  }

  rensa::log(LogLevel::Error, "Unterminated VARS block");

  return vars;
}

Target create_target(const File &file, Iter &it) {
  Target target;

  for (++it; it != file.end(); ++it) {
    auto &[line_number, tokens] = *it;

    if (tokens.empty()) {
      continue;
    }

    if (tokens[0] == "VARS") {
      target.vars = create_vars(file, it);
      continue;
    }

    if (tokens[0] == "END") {
      return target;
    }
  }

  rensa::log(LogLevel::Error, "Unterminated TARGET block");

  return target;
}

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

    if (line.empty()) {
      continue;
    }

    std::stringstream ss(line);

    Vector<String> tokens;

    while (ss >> token) {
      if (token == "#") {
        break;
      }

      tokens.push_back(token);
    }

    if (!tokens.empty()) {
      file[line_number] = std::move(tokens);
    }
  }

  for (auto it = file.begin(); it != file.end(); ++it) {
    const auto &[line_index, tokens] = *it;

    if (tokens.empty()) {
      continue;
    }

    if (tokens[0] == "TARGET") {

      if (tokens.size() < 2) {
        rensa::log(LogLevel::Error, "Target name missing on line '{}'",
                   line_index);

        return SystemStatus::Error;
      }

      if (tokens.size() > 2) {
        rensa::log(LogLevel::Error, "Expected only target name on line '{}'",
                   line_index);

        return SystemStatus::Error;
      }

      const String &name = tokens[1];

      if (targets.contains(name)) {
        rensa::log(LogLevel::Error, "Duplicate target '{}' on line '{}'", name,
                   line_index);

        return SystemStatus::Error;
      }

      targets[name] = create_target(file, it);
      std::println("Target {} created", name);

      continue;
    }

    if (tokens[0] == "END") {
      rensa::log(LogLevel::Error, "Unexpected END directive on line '{}'",
                 line_index);

      return SystemStatus::Error;
    }
  }

  std::println("{}", targets["rensa"].vars);

  return SystemStatus::Success;
}

} // namespace rairen::rensa