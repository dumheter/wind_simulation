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

#include <Math/BsVector3.h>
#include <Math/BsVector3I.h>

// ========================================================================== //
// Types
// ========================================================================== //

namespace wind {

using Vec3F = ::bs::Vector3;
using Vec3I = ::bs::Vector3I;

} // namespace wind

// ========================================================================== //
// Functions
// ========================================================================== //

namespace wind {

/// Clamp 'value' between two other values 'min' and 'max'
template <typename T, typename S, typename U>
inline constexpr T clamp(T value, S min, U max) {
  return value < min ? min : value > max ? max : value;
}

// -------------------------------------------------------------------------- //

/// Returns the maximum of two values
template <typename T> inline constexpr T max(T a, T b) { return a > b ? a : b; }

// -------------------------------------------------------------------------- //

/// Returns the maximum of two values
template <typename T> inline constexpr T max(T a, T b, T c) {
  return wind::max(a, wind::max(b, c));
}

// -------------------------------------------------------------------------- //

/// Returns the minimum of two values
template <typename T> inline constexpr T min(T a, T b) { return a < b ? a : b; }

} // namespace wind