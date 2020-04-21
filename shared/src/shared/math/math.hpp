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

#include "shared/types.hpp"

#include <Math/BsQuaternion.h>
#include <Math/BsVector2.h>
#include <Math/BsVector2I.h>
#include <Math/BsVector3.h>
#include <Math/BsVector3I.h>
#include <Math/BsVector4.h>

#include <alflib/core/assert.hpp>

// ========================================================================== //
// Types
// ========================================================================== //

namespace wind {

using Vec2F = ::bs::Vector2;
using Vec2I = ::bs::Vector2I;
using Vec3F = ::bs::Vector3;
using Vec3I = ::bs::Vector3I;
using Vec4F = ::bs::Vector4;
using Quat = ::bs::Quaternion;

} // namespace wind

// ========================================================================== //
// Functions
// ========================================================================== //

#undef min
#undef max

namespace wind {

/// Clamp 'value' between two other values 'min' and 'max'
template <typename T, typename S, typename U>
constexpr T clamp(T value, S minValue, U maxValue) {
  return value < minValue ? minValue : value > maxValue ? maxValue : value;
}

template <typename T> constexpr T dclamp(T value, T minValue, T maxValue) {
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
template <typename T> T map(T val, T from_min, T from_max, T to_min, T to_max) {
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

// -------------------------------------------------------------------------- //

/// Round a floating-point number to N decimals
inline f32 round(f32 value, u32 dec) {
  const u32 decN = static_cast<u32>(pow(10, dec));
  return roundf(value * decN) / decN;
}

// ============================================================ //

/// How far apart is a and b?
/// @return The sum of position difference
inline f32 distance(Vec3F a, Vec3F b) {
  return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) +
                   (a.z - b.z) * (a.z - b.z));
}

// ============================================================ //

/// https://en.wikipedia.org/wiki/Gaussian_function
inline f32 gaussian(f32 x, f32 scalar, f32 offset, f32 width) {
  const f32 safeWidth = std::max(1.0f, width);
  return scalar *
         std::exp(-((x - offset) * (x - offset)) / (2 * safeWidth * safeWidth));
}

// -------------------------------------------------------------------------- //

/// Calculate standard deviation of list
inline f32 standardDeviation(const std::vector<f32> &values) {
  if (values.empty()) {
    return 0.0f;
  }
  f32 sum = 0.0f;
  f32 sqSum = 0.0f;
  for (size_t i = 0; i < values.size(); i++) {
    sum += values[i];
    sqSum += values[i] * values[i];
  }
  const f32 mean = sum / values.size();
  const f32 variance = (sqSum / values.size()) - (mean * mean);
  return sqrt(variance);
}

// -------------------------------------------------------------------------- //

/// Calculate standard deviation of list
inline f64 standardDeviation(const std::vector<f64> &values) {
  if (values.empty()) {
    return 0.0f;
  }
  f64 sum = 0.0f;
  f64 sqSum = 0.0f;
  for (size_t i = 0; i < values.size(); i++) {
    sum += values[i];
    sqSum += values[i] * values[i];
  }
  const f64 mean = sum / values.size();
  const f64 variance = (sqSum / values.size()) - (mean * mean);
  return sqrt(variance);
}

// -------------------------------------------------------------------------- //

/// Retrieve median (lower and upper) indices for a collection of 'length'
/// values
void medianIndices(size_t length, size_t &lower, size_t &upper);

// -------------------------------------------------------------------------- //

f32 median(const std::vector<f32> &values, size_t left, size_t right);

// -------------------------------------------------------------------------- //

f32 median(const std::vector<f32> &values);

// -------------------------------------------------------------------------- //

f32 quartile1(const std::vector<f32> &values);

// -------------------------------------------------------------------------- //

f32 quartile3(const std::vector<f32> &values);

} // namespace wind
