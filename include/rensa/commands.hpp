#ifndef RAIREN_RENSA_COMMANDS_HPP
#define RAIREN_RENSA_COMMANDS_HPP

#include "rensa/core.hpp"

namespace rairen::rensa {
#define RENSA_COMMAND(name)                                                    \
  SystemStatus name(Orchestrator *orchestrator, Args args)

RENSA_COMMAND(create);

} // namespace rairen::rensa

#endif