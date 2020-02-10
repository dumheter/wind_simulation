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

#include <cstdint>
#include <string>

// ========================================================================== //
// Types
// ========================================================================== //

/** 8-bit character type **/
using char8 = char;
static_assert(sizeof(char8) == 1, "Size of char8 expected to be 1");

#if defined(_WIN32)
/** 16-bit character type **/
using char16 = wchar_t;
#else
/** 16-bit character type **/
using char16 = uint16_t;
#endif
static_assert(sizeof(char16) == 2, "Size of char8 expected to be 2");

/** 8-bit signed type **/
using s8 = int8_t;
/** 8-bit unsigned type **/
using u8 = uint8_t;

/** 16-bit signed type **/
using s16 = int16_t;
/** 16-bit unsigned type **/
using u16 = uint16_t;

/** 32-bit signed type **/
using s32 = int32_t;
/** 32-bit unsigned type **/
using u32 = uint32_t;

/** 64-bit signed type **/
using s64 = int64_t;
/** 8-bit unsigned type **/
using u64 = uint64_t;

/** 32-bit floating-point type **/
using f32 = float;
/** 64-bit floating-point type **/
using f64 = double;

// ========================================================================== //
// Functions
// ========================================================================== //

namespace alflib {

/** Returns the value of the specified bit.
 * \brief Returns bit value.
 * \param index Index of bit.
 * \return Value of bit.
 */
constexpr u64
Bit(u64 index)
{
  return 1ull << index;
}

}