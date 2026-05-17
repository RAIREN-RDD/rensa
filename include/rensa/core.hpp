#ifndef RAIREN_RENSA_CORE_HPP
#define RAIREN_RENSA_CORE_HPP

#include "rensa/types.hpp"

#include <functional>

namespace rairen::rensa {

enum class SystemStatus : i32 { Success = 0, Error = 1, UnknownCommand = -1 };

struct Orchestrator;
using Args = std::span<const String>;

using RensaFunction = std::function<SystemStatus(Orchestrator *, Args)>;

struct Command {
  String help;
  RensaFunction execute;
};

struct Orchestrator {
  OrderedMap<String, Command> commands;
  
  Path current_dir;
  Path proc_path;
  Path templates_path;
  
  Path rensa_dir;
  Path rensa_cache_path;

  Orchestrator();

  SystemStatus dispatch(const Vector<String> &commands);

};

} // namespace rairen::rensa

#endif