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

#include <limits>
#include <string>

#include "shared/types.hpp"

#include <fmt/format.h>

// ========================================================================== //
// String Declaration
// ========================================================================== //

namespace wind {

class String {
public:
  /// Unicode codepoint type
  using Codepoint = u32;

  /// Count to include entire string
  static constexpr u64 kEntireStr = u64(-1);

  /// Construct empty
  String();

  /// Construct from c-str
  String(const char8 *str);

  /// Construct from std::string
  String(const std::string &str);

  /// Find index of byte (8-bit char). -1 for not found
  s64 find(u32 cp) const;

  /// Find index of substring. -1 for not found
  s64 find(const String &substr) const;

  /// Find last index of byte (8-bit char). -1 for not found
  s64 findLast(u32 cp) const;

  /// Retrieve a substring
  String substring(u64 offset, u64 count) const;

  /// Replace codepoints
  void replace(Codepoint from, Codepoint to);

  /// Returns the underlying c-str encoded as UTF-8
  const char8 *getUTF8() const { return m_buf.c_str(); }

  /// Returns the underlying string
  const std::string &getStr() const { return m_buf; }

  /// Returns the size of the string in bytes
  u64 getSize() const { return static_cast<u64>(m_buf.size()); }

  /// Create a formatted string
  template <typename... ARGS>
  static String format(const String &fmt, ARGS &&... args) {
    return fmt::format(fmt.getStr(), std::forward<ARGS>(args)...);
  }

  /// Returns the width of a codepoint in number of units
  static u32 unitCount(Codepoint cp);

  /// Copy a c-string
  static char8 *copyCStr(const char8 *str);

  /// Append other string
  void append(const String &other);

  /// Append other string
  void operator+=(const String &other) { append(other); }

  /// Concatenate strings
  friend String operator+(const String &s0, const String &s1);

  // Equality operator
  friend bool operator==(const String &s0, const String &s1);

  // Inequality operator
  friend bool operator!=(const String &s0, const String &s1);

private:
  /// String buffer
  std::string m_buf;
};

} // namespace wind