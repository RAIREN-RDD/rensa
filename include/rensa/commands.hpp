#pragma once

#include "rensa/core.hpp"

#define RENSA_FUNC(func_name)                                                  \
  rensa::i32 func_name(rensa::Cli *cli, std::span<Argument>)

namespace rensa {

RENSA_FUNC(rensa_version);
RENSA_FUNC(rensa_help);
RENSA_FUNC(rensa_init);

} // namespace rensa
