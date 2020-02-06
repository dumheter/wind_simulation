// MIT License
//
// Copyright (c) 2019 Filip Björklund
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

// Project headers
#include "alflib/core/assert.hpp"
#include "alflib/core/common.hpp"
#include "alflib/platform/detection.hpp"
#include "alflib/math/limits.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace alflib {

/** Returns the minimum of two values.
 * \brief Returns min of two values.
 * \tparam T Type of values.
 * \param a First value.
 * \param b Second value.
 * \return Minimum value.
 */
template<typename T>
constexpr T
Min(T a, T b)
{
  return a < b ? a : b;
}

// -------------------------------------------------------------------------- //

/** Returns the maximum of two values.
 * \brief Returns max of two values.
 * \tparam T Type of values.
 * \param a First value.
 * \param b Second value.
 * \return Maximum value.
 */
template<typename T>
constexpr T
Max(T a, T b)
{
  return a > b ? a : b;
}

// -------------------------------------------------------------------------- //

/** Returns the absolute value of 'a'.
 * \brief Returns absolute value.
 * \tparam T Type of value.
 * \param a Value to get absolute value of.
 * \return Absolute value of 'a'.
 */
template<typename T>
constexpr T
Absolute(T a)
{
  return a > T{ 0 } ? a : -a;
}

// -------------------------------------------------------------------------- //

/** Clamp a value in a range between two other value, 'min' and 'max'.
 * \brief Clamp value.
 * \tparam T Type of value.
 * \param value Value to clamp.
 * \param min Minimum value.
 * \param max Maximum value.
 * \return Clamped value.
 */
template<typename T>
constexpr T
Clamp(T value, T min, T max)
{
  return value < min ? min : value > max ? max : value;
}

// -------------------------------------------------------------------------- //

/** Returns the square root of a value.
 * \brief Returns square root.
 * \param value Value to get square root of.
 * \return Square root value.
 */
inline f32
SquareRoot(f32 value)
{
  return sqrtf(value);
}

// -------------------------------------------------------------------------- //

/** Returns the square root of a value.
 * \brief Returns square root.
 * \param value Value to get square root of.
 * \return Square root value.
 */
inline f64
SquareRoot(f64 value)
{
  return sqrt(value);
}

// -------------------------------------------------------------------------- //

/** Raise a value to a power.
 * \brief Raise value to power.
 * \param value Value to raise.
 * \param power Power to raise the value with.
 * \return Resulting value.
 */
inline f32
Power(f32 value, f32 power)
{
  return powf(value, power);
}

// -------------------------------------------------------------------------- //

/** Raise a value to a power.
 * \brief Raise value to power.
 * \param value Value to raise.
 * \param power Power to raise the value with.
 * \return Resulting value.
 */
inline f64
Power(f64 value, f64 power)
{
  return powf(value, power);
}

// -------------------------------------------------------------------------- //

/** Returns whether or not a value is a power of two.
 * \brief Returns whether value is power of two.
 * \param value Value to check if power of two.
 * \return True if the value is a power of two, otherwise false.
 */
constexpr bool
IsPowerOfTwo(u64 value)
{
  return value && !(value & (value - 1));
}

// -------------------------------------------------------------------------- //

/** Align a value 'base' to a power of two 'alignment'.
 * \brief Align value.
 * \param base Base value to align to the specified alignment.
 * \param alignment Value to align to.
 * \return Aligned value.
 */
inline u64
AlignPowerOfTwo(u64 base, u32 alignment)
{
#if defined(ALFLIB_TARGET_WINDOWS)
#pragma warning(push)
#pragma warning(disable : 4146)
  AlfAssert(alignment > 0 && IsPowerOfTwo(alignment),
            "Alignment must be a power of two");
  return (base + alignment - 1) & -alignment;
#pragma warning(pop)
#else
  AlfAssert(alignment > 0 && IsPowerOfTwo(alignment),
            "Alignment must be a power of two");
  return (base + alignment - 1) & -alignment;
#endif
}

// -------------------------------------------------------------------------- //

/** Compare two floating-point numbers for equality. The machine epsilon is used
 * to correctly compare values even though they may deviate slightly.
 * \brief Compare floats.
 * \param lhs Left-hand side.
 * \param rhs Right-hand side.
 * \return True if the two floating-point numbers are equal otherwise false.
 */
inline bool
FloatEqual(f32 lhs, f32 rhs)
{
  return Absolute(lhs - rhs) < F32_EPSILON;
}

// -------------------------------------------------------------------------- //

/** Compare two floating-point numbers for equality. The machine epsilon is used
 * to correctly compare values even though they may deviate slightly.
 * \brief Compare floats.
 * \param lhs Left-hand side.
 * \param rhs Right-hand side.
 * \return True if the two floating-point numbers are equal otherwise false.
 */
inline bool
FloatEqual(f64 lhs, f64 rhs)
{
  return Absolute(lhs - rhs) < F64_EPSILON;
}

}