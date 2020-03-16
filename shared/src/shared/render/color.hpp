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
  /// Construct RGBA color
  explicit constexpr Color() : m_r(255), m_g(255), m_b(255), m_a(255) {}

  /// Construct RGBA color
  constexpr Color(u8 r, u8 g, u8 b, u8 a = 255)
      : m_r(r), m_g(g), m_b(b), m_a(a) {}

  /// predefined colors
  static constexpr Color black() { return Color{0, 0, 0}; }
  static constexpr Color white() { return Color{255, 255, 255}; }
  static constexpr Color red() {return Color{255, 0, 0}; }
  static constexpr Color green() { return Color{0, 255, 0}; }
  static constexpr Color blue() { return Color{0, 0, 255}; }
  static constexpr Color cyan() { return Color{0, 255, 255}; }
  static constexpr Color magenta() { return Color{255, 0, 255}; }
  static constexpr Color yellow() { return Color{255, 255, 0}; }

  /// Returns the red channel
  constexpr u8 getRed() const { return m_r; }

  /// Returns the red channel
  constexpr u8 &getRed() { return m_r; }

  /// Returns the red channel
  constexpr f32 getRedF32() const { return m_r / 255.0f; }

  /// Returns the green channel
  constexpr u8 getGreen() const { return m_g; }

  /// Returns the green channel
  constexpr u8 &getGreen() { return m_g; }

  /// Returns the green channel
  constexpr f32 getGreenF32() const { return m_g / 255.0f; }

  /// Returns the blue channel
  constexpr u8 getBlue() const { return m_b; }

  /// Returns the blue channel
  constexpr u8 &getBlue() { return m_b; }

  /// Returns the blue channel
  constexpr f32 getBlueF32() const { return m_b / 255.0f; }

  /// Returns the alpha channel
  constexpr u8 getAlpha() const { return m_a; }

  /// Returns the alpha channel
  constexpr u8 &getAlpha() { return m_a; }

  /// Returns the alpha channel
  constexpr f32 getAlphaF32() const { return m_a / 255.0f; }

  /// Make color from float values.
  static constexpr Color makeFloat(f32 r, f32 g, f32 b, f32 a = 1.0f) {
    return Color{u8(r * 255.0f), u8(g * 255.0f), u8(b * 255.0f),
                 u8(a * 255.0f)};
  }

  /// Equality operator
  constexpr friend bool operator==(const Color &c0, const Color &c1) {
    return c0.m_r == c1.m_r && c0.m_g == c1.m_g && c0.m_b == c1.m_b &&
           c0.m_a == c1.m_a;
  }

  /// Inequality operator
  constexpr friend bool operator!=(const Color &c0, const Color &c1) {
    return !(c0 == c1);
  }

private:
  /// Color channels
  u8 m_r, m_g, m_b, m_a;
};

} // namespace wind
