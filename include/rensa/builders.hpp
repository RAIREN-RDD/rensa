#pragma once

#include "rensa/core.hpp"
#include "rensa/types.hpp"


namespace rairen::rensa {

using Vars = UnorderedMap<String, Vector<String>>;

struct Builder {
  virtual ~Builder() = default;
  virtual SystemStatus build(Vars &vars) = 0;
};

struct RensaCpp : Builder {
  SystemStatus build(Vars &vars) override;
};

} // namespace rairen::rensa
