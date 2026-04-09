#include "rensa/commands.hpp"

RENSA_FUNC(rensa::rensa_help) {
  std::ostringstream oss;
  oss << "Usage: rensa <command> [options]\n\n";
  oss << "Commands:\n";
  for (const auto &cmd : cli->get_commands()) {
    oss << std::format("  {}: {}\n", cmd.name, cmd.description);
  }
  oss << "\nProject: https://codeberg.org/rairen-rdd/rensa\n";
  oss << "For irregularities, contact Kai: "
         "https://kai5041.github.io/contact";

  std::println("{}", oss.str());
  return 0;
}
