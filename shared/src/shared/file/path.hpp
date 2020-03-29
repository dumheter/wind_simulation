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

#include "shared/string.hpp"

// ========================================================================== //
// Path Declaration
// ========================================================================== //

namespace wind {

/// Represents a path in the filesystem
class Path {
public:
  /// Enumeration of file extensions
  enum class Ext {
    kNone,    ///< No extension
    kUnknown, /// Unknown extension
    kTxt,     ///< .txt
    kPng,     ///< .png
    kHlsl,    ///< .hlsl
  };

  /// Construct empty
  Path();

  /// Construct from c-string
  Path(const char8 *path);

  /// Construct from String
  Path(const String &path);

  /// Construct from std::string
  Path(const std::string &path);

  /// Join two paths
  void join(const Path &other);

  /// Join two paths
  static Path joined(const Path &p0, const Path &p1);

  /// Returns the path to the parent. This is either the parent directory or the
  /// containing directory of a file.
  Path parent() const;

  String getName() const;

  String getBaseName() const;

  /// Returns the path extension
  String getExtString() const;

  /// Returns the path extension string
  Ext getExt() const;

  /// Returns the path string
  const String &getString() const { return m_str; }

private:
  /// Path string
  String m_str;
};

} // namespace wind