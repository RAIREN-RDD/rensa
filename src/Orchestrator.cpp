#include "rensa/core.hpp"
#include "rensa/log.hpp"
#include "rensa/version.hpp"

#include <print>

namespace rairen::rensa {

Orchestrator::Orchestrator() {
  commands.emplace(
      "version", Command{"Prints Rensa version and commit",
                         [this](Orchestrator *, Args) -> SystemStatus {
                           std::println("rensa {}.{}.{} ({})",
                                        RENSA_VERSION_MAJOR,
                                        RENSA_VERSION_MINOR,
                                        RENSA_VERSION_PATCH, RENSA_COMMIT_HASH);
                           return SystemStatus::Success;
                         }});
}

SystemStatus Orchestrator::dispatch(const Vector<String> &commands) {
  SystemStatus status = SystemStatus::UnknownCommand;

  if (commands.empty()) {
    log(LogLevel::Error,
        "No arguments have been passed. This should not happen.");
    return SystemStatus::Error;
  }

  StringView command_name = commands.at(0);
  RensaFunction func = nullptr;

  for (auto &[cmd_name, cmd] : this->commands) {
    if (command_name == cmd_name) {
      func = cmd.execute;
      break;
    }
  }

  if (func) {
    Args args(commands);
    status = func(this, args.subspan(1));
  }

  return status;
}

} // namespace rairen::rensa