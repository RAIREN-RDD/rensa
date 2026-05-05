#ifndef RAIREN_RENSA_COMMANDS_HPP
#define RAIREN_RENSA_COMMANDS_HPP

#include "rensa/core.hpp"

namespace rairen::rensa {
#define RENSA_COMMAND(name)                                                    \
  SystemStatus name(Orchestrator *orchestrator, Args args)

RENSA_COMMAND(create);
RENSA_COMMAND(build);

RENSA_COMMAND(build_cpp);

} // namespace rairen::rensa

#endif