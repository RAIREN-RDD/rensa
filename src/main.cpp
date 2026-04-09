#include <print>

#include "rensa/core.hpp"
#include "rensa/utils.hpp"

using namespace rensa;

i32 main(i32 argc, char **argv) {
  if (argc == 1) {
    log(LogLevel::ERROR, "No arguments specified");
    return 1;
  }

  StringView command_name = argv[1];
  Vector<Argument> arguments;
  i32 exit_code = 0;

  // Parse arguments and flags
  for (u8 i = 1; i < argc; ++i) {
    StringView arg = argv[i];
    std::size_t separator_pos = arg.find(":");

    if (separator_pos != arg.npos) { // If flag
      Flag flag;
      flag.key = arg.substr(0, separator_pos);
      flag.value = arg.substr(separator_pos + 1);
      arguments.push_back(flag);

    } else { // Primitive argument
      PrimitiveArgument p{arg};
      arguments.push_back(p);
    }
  }

  // Execute
  exit_code = Cli().dispatch(command_name, arguments);

  if (exit_code == -1) {
    log(LogLevel::ERROR,
        "Command '{}' not found. Try 'help' for a list of available commands.",
        command_name);
  }

  return exit_code;
}
