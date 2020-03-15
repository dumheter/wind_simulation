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

#include "color.hpp"

// ========================================================================== //
// Color Implementation
// ========================================================================== //

namespace wind {

Color Color::sBlack = Color{0, 0, 0};

// -------------------------------------------------------------------------- //

Color Color::sWhite = Color{255, 255, 255};

// -------------------------------------------------------------------------- //

Color Color::sRed = Color{255, 0, 0};

// -------------------------------------------------------------------------- //

Color Color::sGreen = Color{0, 255, 0};

// -------------------------------------------------------------------------- //

Color Color::sBlue = Color{0, 0, 255};

// -------------------------------------------------------------------------- //

Color Color::sCyan = Color{0, 255, 255};

// -------------------------------------------------------------------------- //

Color Color::sMagenta = Color{255, 0, 255};

// -------------------------------------------------------------------------- //

Color Color::sYellow = Color{255, 255, 0};

// -------------------------------------------------------------------------- //

Color::Color() : m_r(255), m_g(255), m_b(255), m_a(255) {}

// -------------------------------------------------------------------------- //

Color::Color(u8 r, u8 g, u8 b, u8 a) : m_r(r), m_g(g), m_b(b), m_a(a) {}

// -------------------------------------------------------------------------- //

Color Color::makeFloat(f32 r, f32 g, f32 b, f32 a) {
  return Color{u8(r * 255.0f), u8(g * 255.0f), u8(b * 255.0f), u8(a * 255.0f)};
}

// -------------------------------------------------------------------------- //

bool operator==(const Color &c0, const Color &c1) {
  return c0.m_r == c1.m_r && c0.m_g == c1.m_g && c0.m_b == c1.m_b &&
         c0.m_a == c1.m_a;
}

// -------------------------------------------------------------------------- //

bool operator!=(const Color &c0, const Color &c1) { return !(c0 == c1); }

} // namespace wind