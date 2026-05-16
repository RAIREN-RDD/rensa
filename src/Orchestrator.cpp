#include "rensa/commands.hpp"
#include "rensa/core.hpp"
#include "rensa/log.hpp"
#include "rensa/version.hpp"

#include <filesystem>
#include <print>

#ifdef __linux__
#include <limits.h>
#include <unistd.h>
#else
#error "Unsupported platform"
#endif

namespace rairen::rensa {

Orchestrator::Orchestrator() {
#ifdef __linux__
  char buf[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);

  if (len == -1) {
    log(LogLevel::Error, "Failed to read process path");
    std::exit(-2);
  }

  buf[len] = '\0';
  proc_path = Path(buf).parent_path();
  templates_path = proc_path / "templates";
#endif

  this->current_dir = fs::current_path();
  this->rensa_dir = this->current_dir / ".rensa";
  this->rensa_cache_path = this->rensa_dir / "cache";

  commands.emplace(
      "version", Command{"Prints Rensa version and commit",
                         [this](Orchestrator *, Args) -> SystemStatus {
                           std::println("rensa {}.{}.{} commit {}",
                                        RENSA_VERSION_MAJOR,
                                        RENSA_VERSION_MINOR,
                                        RENSA_VERSION_PATCH, RENSA_COMMIT_HASH);
                           return SystemStatus::Success;
                         }});

  commands.emplace("create",
                   Command{"Create a Rensa project from a template", create});

                   commands.emplace("build", Command{"Build project", build});
  commands.emplace("build-cpp", Command{"Build cpp project", build_cpp});
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