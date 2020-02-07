#pragma once

#include <cstdint>

// ========================================================================== //
// Types
// ========================================================================== //

namespace wind {

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using f32 = float;
using f64 = double;

} // namespace wind

// ========================================================================== //
// Macros
// ========================================================================== //

/* Macro to force a namespace class */
#define WIND_NAMESPACE_CLASS(type)                                             \
public:                                                                        \
  type() = delete;                                                             \
  ~type() = delete;

// ========================================================================== //
// Macros
// ========================================================================== //

namespace wind {

/* Clamp 'value' between two other values 'min' and 'max' */
template <typename T, typename S, typename U>
inline constexpr T Clamp(T value, S min, U max) {
  return value < min ? min : value > max ? max : value;
}

} // namespace wind