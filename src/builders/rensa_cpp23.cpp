#include "rensa/builders.hpp"
#include "rensa/core.hpp"
#include "rensa/log.hpp"

#include <print>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

namespace rairen::rensa {

#define TRY_ASSIGN(v, k)                                                       \
  auto it_##v = vars.find(k);                                                  \
  if (it_##v == vars.end()) {                                                  \
    rensa::log(LogLevel::Error, "Missing variable '{}'", k);                   \
    return SystemStatus::Error;                                                \
  }                                                                            \
  auto v = it_##v->second;

static SystemStatus run(String c, Vector<String> a) {
  pid_t p = fork();
  if (!p) {
    Vector<char *> v;
    v.push_back((char *)c.c_str());
    for (auto &s : a)
      v.push_back((char *)s.c_str());
    v.push_back(nullptr);
    execvp(c.c_str(), v.data());
    _exit(1);
  }
  int st;
  waitpid(p, &st, 0);
  return SystemStatus::Success;
}

#ifdef __linux__

SystemStatus compile_obj(String c, Vector<String> f, String s, String t) {
  f.insert(f.begin(), "-c");
  f.push_back("-o");
  f.push_back(t);
  f.insert(f.begin(), s);
  return run(c, f);
}

SystemStatus link_objs(String c, Vector<Path> s, String t, Vector<String> f) {
  Vector<String> a;
  for (auto &x : s)
    a.push_back(x);
  a.insert(a.end(), f.begin(), f.end());
  a.push_back("-o");
  a.push_back(t);
  return run(c, a);
}

#endif

SystemStatus RensaCpp23::build(Vars &vars) {
  TRY_ASSIGN(target, "TARGET_NAME");
  TRY_ASSIGN(cxx, "CXX");
  TRY_ASSIGN(flags, "CXX_FLAGS");
  TRY_ASSIGN(ld, "CXX_LD_FLAGS");
  TRY_ASSIGN(srcs, "SRC_DIRS");
  TRY_ASSIGN(bdir, "BUILD_DIR");

  for (auto &i : vars["CXX_INCLUDES"])
    flags.push_back("-I" + i);

  Vector<Path> src;
  for (auto &d : srcs)
    for (auto &f : fs::recursive_directory_iterator(d))
      if (f.path().extension() == ".cpp")
        src.push_back(f.path());

  fs::path root = *bdir.begin();
  fs::create_directories(root);

  Vector<Path> obj;
  fs::path cc = *cxx.begin(), bin = root / *target.begin();

  usize i = 1;
  for (auto &s : src) {
    fs::path o = root / fs::relative(s, *srcs.begin());
    o += ".o";
    fs::create_directories(o.parent_path());
    obj.push_back(o);

    std::stringstream ss;
    ss << cc.string();
    for (auto &f : flags)
      ss << " " << f;
    ss << " -o " << o.string();

    std::println("[{}/{}] {}", i++, src.size(), ss.str());
    compile_obj(cc, flags, s, o);
  }

  std::stringstream ss;
  ss << cc.string();
  for (auto &o : obj)
    ss << " " << o;
  for (auto &f : ld)
    ss << " " << f;
  ss << " -o " << bin;

  std::println("{}", ss.str());

  if (link_objs(cc, obj, bin, ld) == SystemStatus::Error) {
    rensa::log(LogLevel::Error, "Failed to link target '{}'", *target.begin());
    return SystemStatus::Error;
  }

  rensa::log(LogLevel::Note, "Target '{}' built succesfully", *target.begin());
  return SystemStatus::Success;
}

} // namespace rairen::rensa
