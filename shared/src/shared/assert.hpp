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

#include "shared/macros.hpp"
#include "shared/string.hpp"
#include "shared/types.hpp"

// ========================================================================== //
// Macros
// ========================================================================== //

///
#define WN_ASSERT(condition, fmt, ...)                                         \
  Assert::cond(condition, #condition, __FILE__, __LINE__, fmt, __VA_ARGS__)

///
#define WN_PANIC(fmt, ...) Assert::panic(__FILE__, __LINE__, fmt, __VA_ARGS__)

// ========================================================================== //
// Assert Declaration
// ========================================================================== //

namespace wind {

WN_FWD_DECLARE(String);

/// Assert functions
class Assert {
  WN_NAMESPACE_CLASS(Assert);

public:
  static void cond(bool cond, const char8 *condStr, const char8 *file, u32 line,
                   const String &msg);

  template <typename... ARGS>
  static void cond(bool cond, const char8 *condStr, const char8 *file, u32 line,
                   const String &fmt, ARGS &&... args) {
    cond(cond, condStr, file, line,
         String::format(fmt, std::forward<ARGS>(args)...));
  }

  WN_NORETURN static void panic(const char8 *file, u32 line, const String &msg);

  template <typename... ARGS>
  WN_NORETURN static void panic(const char8 *file, u32 line, const String &fmt,
                                ARGS &&... args) {
    panic(file, line, String::format(fmt, std::forward<ARGS>(args)...));
  }
};

} // namespace wind