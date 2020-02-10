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
#include "alflib/core/common.hpp"
#include "alflib/platform/detection.hpp"

// ========================================================================== //
// Endianness
// ========================================================================== //

/** Macro for little endian **/
#define ALFLIB_LITTLE_ENDIAN 0x03020100ul
/** Macro for big endian **/
#define ALFLIB_BIG_ENDIAN 0x00010203ul
/** Macro for PDP endian **/
#define ALFLIB_PDP_ENDIAN 0x01000302ul

namespace alflib {

/** Union used to determine host endianness **/
static const union
{
  u8 bytes[4];
  u32 value;
} sHostOrder = { { 0, 1, 2, 3 } };

}

/** Macro for the endianness of the host **/
#define ALFLIB_HOST_ENDIAN (alflib::sHostOrder.value)

// ========================================================================== //
// Linux Headers
// ========================================================================== //

#if defined(ALFLIB_TARGET_LINUX)

#include <csignal>

#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#endif

// ========================================================================== //
// x86 Headers
// ========================================================================== //

#if defined(ALFLIB_ARCH_AMD64)

#include <immintrin.h>

#endif
