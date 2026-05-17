#ifndef RAIREN_RENSA_TYPES_HPP
#define RAIREN_RENSA_TYPES_HPP

#include <map>
#include <optional>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>

namespace rairen::rensa {

using u8 = std::uint8_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i32 = std::int32_t;

using usize = std::size_t;

using String = std::string;
using StringView = std::string_view;

template <typename _T> using Vector = std::vector<_T>;
template <typename _K, typename _V> using OrderedMap = std::map<_K, _V>;
template <typename _K, typename _V> using UnorderedMap = std::unordered_map<_K, _V>;
template <typename _T> using UnorderedSet = std::unordered_set<_T>;

template <typename _T> using Optional = std::optional<_T>;

namespace fs = std::filesystem;
using Path = fs::path;

} // namespace rairen::rensa

#endif