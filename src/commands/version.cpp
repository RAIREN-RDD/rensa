#include "rensa/version.hpp"
#include "rensa/commands.hpp"

#include <print>

namespace rairen::rensa {

RENSA_COMMAND(version) {
  std::println("rensa {}.{}.{} commit {}", RENSA_VERSION_MAJOR,
               RENSA_VERSION_MINOR, RENSA_VERSION_PATCH, RENSA_COMMIT_HASH);
  return SystemStatus::Success;
}

} // namespace rairen::rensa
