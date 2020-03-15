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

#include "shared/types.hpp"

// ========================================================================== //
// Color Declaration
// ========================================================================== //

namespace wind {

/// Class that represents an RGBA color
class Color {
public:
  /// Black
  static Color sBlack;
  /// White
  static Color sWhite;

  /// Red
  static Color sRed;
  /// Green
  static Color sGreen;
  /// Blue
  static Color sBlue;

  /// Cyan
  static Color sCyan;
  /// Magenta
  static Color sMagenta;
  /// Yellow
  static Color sYellow;

  /// Construct RGBA color
  explicit Color();

  /// Construct RGBA color
  Color(u8 r, u8 g, u8 b, u8 a = 255);

  /// Returns the red channel
  u8 getRed() const { return m_r; }

  /// Returns the red channel
  u8 &getRed() { return m_r; }

  /// Returns the red channel
  f32 getRedF32() const { return m_r / 255.0f; }

  /// Returns the green channel
  u8 getGreen() const { return m_g; }

  /// Returns the green channel
  u8 &getGreen() { return m_g; }

  /// Returns the green channel
  f32 getGreenF32() const { return m_g / 255.0f; }

  /// Returns the blue channel
  u8 getBlue() const { return m_b; }

  /// Returns the blue channel
  u8 &getBlue() { return m_b; }

  /// Returns the blue channel
  f32 getBlueF32() const { return m_b / 255.0f; }

  /// Returns the alpha channel
  u8 getAlpha() const { return m_a; }

  /// Returns the alpha channel
  u8 &getAlpha() { return m_a; }

  /// Returns the alpha channel
  f32 getAlphaF32() const { return m_a / 255.0f; }

  /// Make color from float values.
  static Color makeFloat(f32 r, f32 g, f32 b, f32 a = 1.0f);

  /// Equality operator
  friend bool operator==(const Color &c0, const Color &c1);

  /// Inequality operator
  friend bool operator!=(const Color &c0, const Color &c1);

private:
  /// Color channels
  u8 m_r, m_g, m_b, m_a;
};

} // namespace wind