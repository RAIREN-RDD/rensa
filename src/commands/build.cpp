#include "rensa/builders.hpp"
#include "rensa/commands.hpp"
#include "rensa/core.hpp"
#include "rensa/log.hpp"

#include <cwctype>
#include <expected>
#include <fstream>

#include <print>
#include <yaml-cpp/yaml.h>

namespace rairen::rensa {

struct Dependency {
  virtual ~Dependency() = default;
  virtual String id() const = 0;
};

struct SourceDependency : Dependency {
  Path path;
  String id() const override { return this->path; };
}; // raw files to process
 

struct TargetDependency : Dependency {
  String target_name;
  String id() const override { return this->target_name; };
}; // standalone external targets

struct Target {
  String name;
  UnorderedMap<String, String> vars;

  Vector<Dependency*> dependencies;
};

/* ----- COMMAND BUILD ----- */
RENSA_COMMAND(build) {
  UnorderedMap<String, String> targets;

  String target_name = args.size() > 1 ? args[1] : "";

  std::ifstream file_in("build.rensa");

  if (!file_in.is_open()) {
    rensa::log(LogLevel::Error, "Failed to open 'build.rensa'");
    return SystemStatus::Error;
  }

  SystemStatus out = SystemStatus::Error;

  std::ifstream file;

  file.open("build.rensa");

  String line;

  while (std::getline(file, line)) {
    for (usize i = 0; i < line.size(); i++) {

      // Safe ASCII whitespace skipping
      if (std::isspace(line[i], std::locale::classic()) ) {
       continue; 
      }

    }

  }


  return out;
}

} // namespace rairen::rensa
