// MIT License
//
// Copyright (c) 2020 Filip Bj�rklund, Christoffer Gustafsson
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
#include "shared/math/math.hpp"

// ========================================================================== //
// Debug Declaration
// ========================================================================== //

namespace wind {

///	Class with debug utilities
class Debug {
  WIND_NAMESPACE_CLASS(Debug);

public:
  /// Break the debugger
  static void breakDebugger();

  /// Check that the specified value is in range otherwise break the debugger
  template <typename T>
  static void rangeCheck(T value, T minValue, T maxValue) {
    if (value < minValue || value > maxValue) {
      breakDebugger();
    }
  }

  /// Clamp a value between the min and max values. If the value is outside the
  /// range the debugger breaks
  template <typename T> static T clampDebug(T value, T minValue, T maxValue) {
    rangeCheck(value, minValue, maxValue);
    return clamp(value, minValue, maxValue);
  }
};

} // namespace wind