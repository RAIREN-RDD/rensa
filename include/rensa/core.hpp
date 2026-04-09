#pragma once

#include <functional>
#include <span>
#include <variant>

#include "rensa/utils.hpp"

namespace rensa {

class Cli;

struct Flag {
  StringView key;
  std::variant<StringView, std::initializer_list<String>> value;
};

struct PrimitiveArgument {
  StringView name;
};

using Argument = std::variant<PrimitiveArgument, Flag>;
using Arguments = Vector<Argument>;

struct Command {
  StringView name;
  std::move_only_function<i32(Cli *, std::span<Argument>)> exec;
  String description = "no description";
  String help = "no help";
};

using Commands = Vector<Command>;

class Cli {
private:
  Commands commands;

public:
  Cli();

  i32 dispatch(StringView command_name, std::span<Argument> args);

  std::span<Command> get_commands() { return this->commands; }
};

} // namespace rensa