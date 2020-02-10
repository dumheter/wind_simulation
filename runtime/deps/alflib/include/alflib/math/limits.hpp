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
// Constants
// ========================================================================== //

namespace alflib {

/** Minimum value of 's8' **/
static constexpr s8 S8_MIN = -128;
/** Maximum value of 's8' **/
static constexpr s8 S8_MAX = 127;

/** Maximum value of 'u8' **/
static constexpr u8 U8_MAX = 255;

/** Minimum value of 's16' **/
static constexpr s16 S16_MIN = -32768;
/** Maximum value of 's16' **/
static constexpr s16 S16_MAX = 32767;

/** Maximum value of 'u16' **/
static constexpr u16 U16_MAX = 65535;

/** Minimum value of 's32' **/
static constexpr s32 S32_MIN = -2147483648;
/** Maximum value of 's32' **/
static constexpr s32 S32_MAX = 2147483647;

/** Maximum value of 'u32' **/
static constexpr u32 U32_MAX = 4294967295u;

/** Minimum value of 's64' **/
static constexpr s64 S64_MIN = -9223372036854775807L - 1;
/** Maximum value of 's64' **/
static constexpr s64 S64_MAX = 9223372036854775807;

/** Maximum value of 'u64' **/
static constexpr u64 U64_MAX = 18446744073709551615ull;

/** Machine epsilon for 'f32' **/
static constexpr f32 F32_EPSILON = 1.192093e-07;
/** Machine epsilon for 'f64' **/
static constexpr f64 F64_EPSILON = 2.220446e-16;

}