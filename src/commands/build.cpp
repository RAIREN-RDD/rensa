#include "rensa/commands.hpp"
#include "rensa/core.hpp"
#include "rensa/log.hpp"

#include <filesystem>
#include <fstream>
#include <print>
#include <sstream>
#include <string>
#include <unordered_set>

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

enum class TokenType {
  Identifier,
  Keyword,
  StringLiteral,

  Operator,
  BlockStart, // TARGET, VARS, DEPENDENCIES
  BlockEnd,   // END

  VariableRef, // ${PROJECT_NAME}

  EndOfFile,
  NewLine
};
class Token {
public:
  TokenType type;
  std::string value;
  u64 line_number;

  Token(TokenType _type, std::string _value, u64 _line_number)
      : type(_type), value(std::move(_value)), line_number(_line_number) {}
};

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

  String line;

  Statuses statuses;
  usize line_number = 0;

  UnorderedSet<String> keywords = {"PROJECT_NAME", "PROJECT_VERSION"};
  UnorderedSet<String> block_starts = {"TARGET", "VARS", "DEPENDENCIES"};

  UnorderedSet<String> operators = {"=", "+=", "?="};

  Vector<Token> tokens;
  String token;

  bool inside_var = false;

  while (std::getline(f, line)) {
    line_number++;

    if (line.empty())
      continue;

    std::stringstream iss(line);
    while (iss >> token) {
      if (token == "//")
        break;

      bool is_keyword = keywords.contains(token);
      if (is_keyword) {
        tokens.push_back(Token(TokenType::Keyword, token, line_number));
        continue;
      }

      bool is_block = block_starts.contains(token);
      if (is_block) {
        tokens.push_back(Token(TokenType::BlockStart, token, line_number));
        if (token == "VARS") {
          inside_var = true;
        }
        continue;
      }

      bool is_operator = operators.contains(token);
      if (is_operator) {
        tokens.push_back(Token(TokenType::Operator, token, line_number));
        continue;
      }

      if (inside_var) {
        tokens.push_back(Token(TokenType::Identifier, token, line_number));
        continue;
      }

      if (token == "END") {
        tokens.push_back(Token(TokenType::BlockEnd, "", line_number));
        inside_var = false;
        continue;
      }

      StringView token_view(token);
      if (token_view.starts_with("RS{")) {
        if (!token_view.ends_with("}")) {
          rensa::log(LogLevel::Error,
                     "RS expression at line {} not closed - '}}' missing",
                     line_number);
          return SystemStatus::Error;
        }

        tokens.push_back(Token(
            TokenType::VariableRef,
            String(token_view.substr(3, token_view.size() - 4)), line_number));

        continue;
      }

      tokens.push_back(Token(TokenType::StringLiteral, token, line_number));
    }
  }

  for (auto token : tokens) {
    std::println("{} = {}", int(token.type), token.value);
  }

  return SystemStatus::Success;
}

} // namespace rairen::rensa