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
// Platform Detection
// ========================================================================== //

#if defined(_WIN32)
/** Microsoft Windows target platform **/
#define ALFLIB_TARGET_WINDOWS
#if defined(_M_AMD64)
/** 64-bit x86 target architecture **/
#define ALFLIB_ARCH_AMD64
#elif defined(_M_ARM64)
/** 64-bit ARM target architecture **/
#define ALFLIB_ARCH_ARM64
#else
#error Invalid Windows target architecture (Supported: x86_64, ARM64)
#endif
#elif defined(__linux__) && !defined(__ANDROID__)
/** Linux target platform **/
#define ALFLIB_TARGET_LINUX
#if defined(__x86_64__)
/** 64-bit x86 target architecture **/
#define ALFLIB_ARCH_AMD64
#elif defined(__aarch64__)
/** 64-bit ARM target architecture **/
#define ALFLIB_ARCH_ARM64
#else
#error Invalid Linux target architecture (Supported: x86_64, ARM64)
#endif
#elif defined(__APPLE__)
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
/** Apple iOS simulator target platform **/
#define ALFLIB_TARGET_IOS_SIMULATOR
#elif TARGET_OS_IPHONE
/** Apple iOS target platform **/
#define ALFLIB_TARGET_IOS
/** 64-bit ARM target architecture **/
#define ALFLIB_ARCH_ARM64
#elif TARGET_OS_MAC
/** Apple MacOS target platform **/
#define ALFLIB_TARGET_MACOS
#if defined(__x86_64__)
/** 64-bit x86 target architecture **/
#define ALFLIB_ARCH_AMD64
#else
#error Invalid MacOS target architecture (Supported: x86_64)
#endif
#else
#error "Unknown Apple OS"
#endif
#elif defined(__ANDROID__)
#define ALFLIB_TARGET_ANDROID
#endif