#include "rensa/commands.hpp"

#include "rensa/version.hpp"

RENSA_FUNC(rensa::rensa_version) {
  std::println("rensa {}.{}.{} commit {}", RENSA_VERSION_MAJOR,
               RENSA_VERSION_MINOR, RENSA_VERSION_PATCH, RENSA_COMMIT_HASH);
  return 0;
}
