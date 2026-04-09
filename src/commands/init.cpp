#include "rensa/commands.hpp"
#include "rensa/utils.hpp"

RENSA_FUNC(rensa::rensa_init) {
  Path current_dir = fs::current_path();

  if (fs::exists(".rensa")) {
    if (!fs::is_directory(".rensa")) {
      log(LogLevel::ERROR,
          "'.resna' is a file. Remove to initialize properly.");
      return 1;
    }

    log(LogLevel::ERROR, "Already a Rensa project.");
    return 2;
  }

  fs::create_directories(current_dir / ".rensa");
  return 0;
}