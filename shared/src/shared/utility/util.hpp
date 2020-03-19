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
#include "shared/types.hpp"

#include <RenderAPI/BsRenderWindow.h>
#include <Scene/BsSceneObject.h>
#include <dlog/dlog.hpp>

// ========================================================================== //
// Util Declaration
// ========================================================================== //

namespace wind {

///
///
class Util {
  WIND_NAMESPACE_CLASS(Util);

public:
  /// Center the cursor in the middle of a window. Specifying nullptr as the
  /// window is the same as specifying the primary window
  static void centerCursor(bs::SPtr<bs::RenderWindow> window = nullptr);

  /// Dump an entire scene. The traversal starts from the specified scene and
  /// then is recursively done down into the children.
  static void dumpScene(const bs::HSceneObject &scene);

  /// Repeat a character N times
  static std::string repeat(char c, u32 n);

  /// Generate random UUID
  static bs::UUID randUUID();

  /// Returns whether a file exists or not
  static bool fileExist(const String &path);

  /// Returns whether a file exists or not
  static bool fileExist(const bs::Path &path);

  /// Read an entire file to a string
  static String readFile(const String &path);

  /// Write a string to a file
  static void writeFile(const String &path, const String &text);

  /// Filter a 'pattern' from 'string'
  static String filter(const String &string, const String &pattern);

  /// Replace characters in string
  static String replace(const String &string, char from, char to);

  template <typename... ARGS>
  [[noreturn]] static void panic(const String &message, ARGS &&... arguments) {
    DLOG_ERROR(message, std::forward<ARGS>(arguments)...);
    abort();
  }
};

// -------------------------------------------------------------------------- //

/// Set the @bitPos in the @store to @bitValue.
template <typename T>
inline constexpr void bitSet(T &store, T bitPos, T bitValue) {
  store ^= (-bitValue ^ store) & (T{1} << bitPos);
}

// -------------------------------------------------------------------------- //

/// Check what value bit @bitPos has in @store.
template <typename T> inline constexpr T bitCheck(const T &store, T bitPos) {
  return (store >> bitPos) & T{1};
}

} // namespace wind
