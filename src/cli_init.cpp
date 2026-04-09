#include <format>
#include <print>

#include "rensa/commands.hpp"
#include "rensa/core.hpp"

rensa::Cli::Cli() {

  commands.push_back(Command{"version", rensa_version,
                             "Print project version and commit hash"});

  commands.push_back(
      Command{"help", rensa_help,
              "Prints this message. Add <command> for detailed related usage "
              "instructions."});

  commands.push_back(
      Command{"init", rensa_init,
              "Initialize the current directory as a Rensa project."});
}
