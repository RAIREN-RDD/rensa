#include "rensa/core.hpp"

rensa::i32 rensa::Cli::dispatch(StringView command_name,
                                std::span<Argument> args) {

  // O_sh(n * O_s(1) + O_h(1))
  for (Command &cmd : commands) {
    if (command_name == cmd.name) {
      return cmd.exec(this, args);
    }
  }

  return -1; // command not found
}
