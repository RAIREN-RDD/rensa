#ifndef RAIREN_RENSA_TYPES_HPP
#define RAIREN_RENSA_TYPES_HPP

#include <cstdint>
#include <filesystem>
#include <map>
#include <vector>

namespace rairen::rensa {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i32 = std::int32_t;

using usize = std::size_t;

using String = std::string;
using StringView = std::string_view;

template <typename _T> using Vector = std::vector<_T>;
template <typename _K, typename _V> using Map = std::map<_K, _V>;

namespace fs = std::filesystem;
using Path = fs::path;

} // namespace rairen::rensa

#endif