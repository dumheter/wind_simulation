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

#include "alflib/graphics/color.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "alflib/math/limits.hpp"
#include "alflib/math/math.hpp"

// ========================================================================== //
// Color Implementation
// ========================================================================== //

namespace alflib {

Color Color::BLACK = Color(0, 0, 0);

// -------------------------------------------------------------------------- //

Color Color::WHITE = Color(U8_MAX, U8_MAX, U8_MAX);

// -------------------------------------------------------------------------- //

Color Color::RED = Color(U8_MAX, u8(0), u8(0));

// -------------------------------------------------------------------------- //

Color Color::GREEN = Color(u8(0), U8_MAX, u8(0));

// -------------------------------------------------------------------------- //

Color Color::BLUE = Color(u8(0), u8(0), U8_MAX);

// -------------------------------------------------------------------------- //

Color Color::CYAN = Color(u8(0), U8_MAX, U8_MAX);

// -------------------------------------------------------------------------- //

Color Color::MAGENTA = Color(U8_MAX, u8(0), U8_MAX);

// -------------------------------------------------------------------------- //

Color Color::YELLOW = Color(U8_MAX, U8_MAX, u8(0));

// -------------------------------------------------------------------------- //

Color Color::CORNFLOWER_BLUE = Color(100, 149, 237);

// -------------------------------------------------------------------------- //

Color::Color(u8 red, u8 green, u8 blue, u8 alpha)
  : mRed(red)
  , mGreen(green)
  , mBlue(blue)
  , mAlpha(alpha)
{}

// -------------------------------------------------------------------------- //

Color::Color(s32 red, s32 green, s32 blue, s32 alpha)
  : mRed(red)
  , mGreen(green)
  , mBlue(blue)
  , mAlpha(alpha)
{}

// -------------------------------------------------------------------------- //

Color::Color(u32 red, u32 green, u32 blue, u32 alpha)
  : mRed(red)
  , mGreen(green)
  , mBlue(blue)
  , mAlpha(alpha)
{}

// -------------------------------------------------------------------------- //

Color::Color(f32 red, f32 green, f32 blue, f32 alpha)
  : mRed(Clamp(u8(red * U8_MAX), u8(0), U8_MAX))
  , mGreen(Clamp(u8(green * U8_MAX), u8(0), U8_MAX))
  , mBlue(Clamp(u8(blue * U8_MAX), u8(0), U8_MAX))
  , mAlpha(Clamp(u8(alpha * U8_MAX), u8(0), U8_MAX))
{}

// -------------------------------------------------------------------------- //

Color::Color(u32 combined)
  : Color((combined >> 24u) & 0xFFu,
          (combined >> 16u) & 0xFFu,
          (combined >> 8u) & 0xFFu,
          (combined >> 0u) & 0xFFu)
{}

// -------------------------------------------------------------------------- //

Color
Color::ToLinear()
{
  f32 red = GetRedF32();
  f32 green = GetRedF32();
  f32 blue = GetRedF32();
  f32 alpha = GetRedF32();

  return Color{
    red <= 0.04045f ? red / 12.92f : Power((red + 0.055f) / 1.055f, 2.4f),
    green <= 0.04045f ? green / 12.92f : Power((green + 0.055f) / 1.055f, 2.4f),
    blue <= 0.04045f ? blue / 12.92f : Power((blue + 0.055f) / 1.055f, 2.4f),
    alpha
  };
}

// -------------------------------------------------------------------------- //

Color
Color::ToGamma()
{
  f32 red = GetRedF32();
  f32 green = GetRedF32();
  f32 blue = GetRedF32();
  f32 alpha = GetRedF32();

  return Color{
    red <= 0.0031308f ? 12.92f * red : 1.055f * Power(red, 1.0f / 2.4f),
    green <= 0.0031308f ? 12.92f * green : 1.055f * Power(green, 1.0f / 2.4f),
    blue <= 0.0031308f ? 12.92f * blue : 1.055f * Power(blue, 1.0f / 2.4f),
    alpha
  };
}

// -------------------------------------------------------------------------- //

Color
Color::ToGrayscale()
{
  f32 red = GetRedF32();
  f32 green = GetRedF32();
  f32 blue = GetRedF32();
  const f32 gray = red * 0.2126f + green * 0.7152f + blue * 0.0722f;
  return Color{ gray, gray, gray };
}

// -------------------------------------------------------------------------- //

u32
Color::GetCombined()
{
  u32 combined = 0;
  combined &= (u32(mRed * U8_MAX) << 24u);
  combined &= (u32(mGreen * U8_MAX) << 16u);
  combined &= (u32(mBlue * U8_MAX) << 8u);
  combined &= (u32(mAlpha * U8_MAX) << 0u);
  return combined;
}

// -------------------------------------------------------------------------- //

bool
operator==(const Color& lhs, const Color& rhs)
{
  return lhs.mRed == rhs.mRed && lhs.mGreen == rhs.mGreen &&
         lhs.mBlue == rhs.mBlue && lhs.mAlpha == rhs.mAlpha;
}

// -------------------------------------------------------------------------- //

bool
operator!=(const Color& lhs, const Color& rhs)
{
  return lhs.mRed != rhs.mRed || lhs.mGreen != rhs.mGreen ||
         lhs.mBlue != rhs.mBlue || lhs.mAlpha != rhs.mAlpha;
}

// -------------------------------------------------------------------------- //

std::ostream&
operator<<(std::ostream& os, const Color& color)
{
  os << "Color {" << u32(color.mRed) << ", " << u32(color.mGreen) << ", "
     << u32(color.mBlue) << ", " << u32(color.mAlpha) << "}";
  return os;
}

}