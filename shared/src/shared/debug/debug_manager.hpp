// MIT License
//
// Copyright (c) 2019-2020 Filip Björklund, Christoffer Gustafsson
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

#include <unordered_map>

// ========================================================================== //
// DebugManager Declaration
// ========================================================================== //

namespace wind {

/// Debug manager. Has global debug variables.
class DebugManager {
public:
  /// Get s32.
  static s32 getS32(const String &name);

  /// Set s32.
  static void setS32(const String &name, s32 value);

  /// Get f32.
  static f32 getF32(const String &name);

  /// Set f32.
  static void setF32(const String &name, f32 value);

  /// Get bool.
  static bool getBool(const String &name);

  /// Set bool.
  static void setBool(const String &name, bool value);

  /// Get String.
  static String getString(const String &name);

  /// Set String.
  static void setString(const String &name, const String &value);

  /// Returns the debug manager instance.
  static DebugManager &instance();

private:
  DebugManager() = default;

  ~DebugManager() = default;

  /// S32 map
  std::unordered_map<String, s32> m_mapS32;
  /// F32 map
  std::unordered_map<String, f32> m_mapF32;
  /// Bool map
  std::unordered_map<String, bool> m_mapBool;
  /// String map
  std::unordered_map<String, String> m_mapString;
};

} // namespace wind