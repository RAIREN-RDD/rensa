#include "rensa/core.hpp"
#include "rensa/log.hpp"
#include "rensa/types.hpp"
#include <print>

using namespace rairen::rensa;

i32 main(int argc, char **argv) {
  if (argc < 2) {
    log(LogLevel::Error, "Not enough arguments");
    return 1;
  }

  Vector<String> args(argv + 1, argv + argc);

  SystemStatus status = Orchestrator().dispatch(args);

  if (status == SystemStatus::UnknownCommand) {
    log(LogLevel::Error, "Unknown command '{}'", args[0]);
  }

  return static_cast<i32>(status);
}