// MIT License
//
// Copyright (c) 2020 Filip Björklund, Christoffer Gustafsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <Math/BsVector2.h>
#include <Math/BsVector3.h>
#include <Math/BsVector3I.h>

// ========================================================================== //
// Types
// ========================================================================== //

namespace wind {

using Vec2F = ::bs::Vector2;
using Vec3F = ::bs::Vector3;
using Vec3I = ::bs::Vector3I;

} // namespace wind

// ========================================================================== //
// Functions
// ========================================================================== //

namespace wind {

/// Clamp 'value' between two other values 'min' and 'max'
template <typename T, typename S, typename U>
inline constexpr T clamp(T value, S minValue, U maxValue) {
  return value < minValue ? minValue : value > maxValue ? maxValue : value;
}

// ============================================================ //

/**
 * Map val from a range [from_min, from_max], to another range
 * [to_min, to_max].
 *
 * @pre val must be in range [from_min, from_max].
 *
 * Example:
 *   Map(5, 0, 10, 0, 100)  -> 50
 *   Map(10, 0, 10, 0, 100) -> 100
 *   Map(0, 0, 10, 0, 100)  -> 0
 */
template <typename T>
inline T map(T val, T from_min, T from_max, T to_min, T to_max) {
  return (val - from_min) * (to_max - to_min) / (from_max - from_min) + to_min;
}

// -------------------------------------------------------------------------- //

/// Returns the maximum of two values
template <typename T> inline constexpr T maxValue(T a, T b) {
  return a > b ? a : b;
}

// -------------------------------------------------------------------------- //

/// Returns the maximum of two values
template <typename T> inline constexpr T maxValue(T a, T b, T c) {
  return wind::maxValue(a, wind::maxValue(b, c));
}

// -------------------------------------------------------------------------- //

/// Returns the minimum of two values
template <typename T> inline constexpr T minValue(T a, T b) {
  return a < b ? a : b;
}

} // namespace wind
