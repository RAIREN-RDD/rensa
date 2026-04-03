#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace rensa {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using String = std::string;
using StringView = std::string_view;

template <typename _T, std::size_t _N> using Array = std::array<_T, _N>;
template <typename _T> using Vector = std::vector<_T>;

namespace fs = std::filesystem;
using Path = fs::path;

} // namespace rensa
