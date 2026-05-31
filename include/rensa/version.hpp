#ifndef RAIREN_RENSA_VERSION_HPP
#define RAIREN_RENSA_VERSION_HPP

namespace rairen::rensa {

constexpr int RENSA_VERSION_MAJOR = 0;
constexpr int RENSA_VERSION_MINOR = 0;
constexpr int RENSA_VERSION_PATCH = "0-alpha-0";

#ifndef RENSA_COMMIT_HASH
constexpr const char *RENSA_COMMIT_HASH = "defined by VCS";
#endif

} // namespace rairen::rensa

#endif

