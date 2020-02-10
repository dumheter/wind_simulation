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

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

// Standard headers
#include <ostream>

// Project headers
#include "alflib/core/common.hpp"
#include "alflib/math/limits.hpp"

// ========================================================================== //
// Color Declaration
// ========================================================================== //

namespace alflib {

/** \class Color
 * \author Filip Björklund
 * \date 22 june 2019 - 08:43
 * \brief Color in RGBA format.
 * \details
 * Class that represents a color with channels for 'red', 'blue', 'green' and
 * 'alpha'.
 */
class Color
{
public:
  /** Black color constant **/
  static Color BLACK;
  /** White color constant **/
  static Color WHITE;

  /** Red color constant **/
  static Color RED;
  /** Green color constant **/
  static Color GREEN;
  /** Blue color constant **/
  static Color BLUE;
  /** Cyan color constant **/
  static Color CYAN;
  /** Magenta color constant **/
  static Color MAGENTA;
  /** Yellow color constant **/
  static Color YELLOW;

  /** Cornflower blue color constant **/
  static Color CORNFLOWER_BLUE;

private:
  /** Value of red channel (0 -> 255) **/
  u8 mRed;
  /** Value of green channel (0 -> 255) **/
  u8 mGreen;
  /** Value of blue channel (0 -> 255) **/
  u8 mBlue;
  /** Value of alpha channel (0 -> 255) **/
  u8 mAlpha;

public:
  /** Construct a color from values for the four (4) separate color channels.
   * \brief Construct color.
   * \param red Red channel value.
   * \param green Green channel value.
   * \param blue Blue channel value.
   * \param alpha Alpha channel value.
   */
  Color(u8 red, u8 green, u8 blue, u8 alpha = U8_MAX);

  /** Construct a color from values for the four (4) separate color channels.
   * \brief Construct color.
   * \param red Red channel value.
   * \param green Green channel value.
   * \param blue Blue channel value.
   * \param alpha Alpha channel value.
   */
  Color(s32 red, s32 green, s32 blue, s32 alpha = U8_MAX);

  /** Construct a color from values for the four (4) separate color channels.
   * \brief Construct color.
   * \param red Red channel value.
   * \param green Green channel value.
   * \param blue Blue channel value.
   * \param alpha Alpha channel value.
   */
  Color(u32 red, u32 green, u32 blue, u32 alpha = U8_MAX);

  /** Construct a color from values for the four (4) separate color channels.
   * The values are transformed from the range 0.0 -> 1.0. Values outside this
   * range are clamped.
   * \brief Construct color.
   * \param red Red channel value.
   * \param green Green channel value.
   * \param blue Blue channel value.
   * \param alpha Alpha channel value.
   */
  Color(f32 red, f32 green, f32 blue, f32 alpha = 1.0f);

  /** Construct a color from a combined 32-bit integer holding each channel.
   * The red channel is stored in the most-significant bits and the alpha in
   * the least-significant bits.
   * \brief Construct color from combined value.
   * \param combined Combined color value.
   */
  explicit Color(u32 combined);

  /** Convert the color from gamma (sRGB) color space to linear color space.
   * \brief Convert from gamma to linear.
   * \return Linear color
   */
  Color ToLinear();

  /** Convert the color from linear color space to gamma (sRGB) color space.
   * \brief Convert from linear to gamma.
   * \return Gamma color
   */
  Color ToGamma();

  /** Convert the color to grayscale.
   * \brief Convert to grayscale.
   * \return Grayscale color.
   */
  Color ToGrayscale();

  /** Returns a combined value for all the color channels.
   * \brief Returns combined color value.
   * \return Combined color value.
   */
  u32 GetCombined();

  /** Returns the red channel.
   * \brief Returns red channel.
   * \return Red channel.
   */
  u8& Red() { return mRed; }

  /** Returns the red channel.
   * \brief Returns red channel.
   * \return Red channel.
   */
  const u8& Red() const { return mRed; }

  /** Returns the green channel.
   * \brief Returns green channel.
   * \return Green channel.
   */
  u8& Green() { return mGreen; }

  /** Returns the green channel.
   * \brief Returns green channel.
   * \return Green channel.
   */
  const u8& Green() const { return mGreen; }

  /** Returns the blue channel.
   * \brief Returns blue channel.
   * \return Blue channel.
   */
  u8& Blue() { return mBlue; }

  /** Returns the blue channel.
   * \brief Returns blue channel.
   * \return Blue channel.
   */
  const u8& Blue() const { return mBlue; }

  /** Returns the alpha channel.
   * \brief Returns alpha channel.
   * \return Alpha channel.
   */
  u8& Alpha() { return mAlpha; }

  /** Returns the alpha channel.
   * \brief Returns alpha channel.
   * \return Alpha channel.
   */
  const u8& Alpha() const { return mAlpha; }

  /** Returns the value of the red channel mapped to the range 0.0 -> 1.0
   * \brief Returns red channel as 'f32'.
   * \return Red channel.
   */
  f32 GetRedF32() const { return mRed / f32(U8_MAX); }

  /** Returns the value of the green channel mapped to the range 0.0 -> 1.0
   * \brief Returns green channel as 'f32'.
   * \return Green channel.
   */
  f32 GetGreenF32() const { return mGreen / f32(U8_MAX); }

  /** Returns the value of the blue channel mapped to the range 0.0 -> 1.0
   * \brief Returns blue channel as 'f32'.
   * \return Blue channel.
   */
  f32 GetBlueF32() const { return mBlue / f32(U8_MAX); }

  /** Returns the value of the alpha channel mapped to the range 0.0 -> 1.0
   * \brief Returns alpha channel as 'f32'.
   * \return Alpha channel.
   */
  f32 GetAlphaF32() const { return mAlpha / f32(U8_MAX); }

public:
  /** Compare two colors for equality.
   * \brief Color equality comparison.
   * \param lhs Left-hand side.
   * \param rhs Right-hand side.
   * \return True if the two colors are equals otherwise false.
   */
  friend bool operator==(const Color& lhs, const Color& rhs);

  /** Compare two colors for inequality.
   * \brief Color inequality comparison.
   * \param lhs Left-hand side.
   * \param rhs Right-hand side.
   * \return False if the two colors are equals otherwise true.
   */
  friend bool operator!=(const Color& lhs, const Color& rhs);

  /** Function for writing a 'Color' to an output stream.
   * \brief Write 'Color' to output stream.
   * \param os Output stream.
   * \param result Result to write to stream.
   * \return Output stream.
   */
  friend std::ostream& operator<<(std::ostream& os, const Color& color);
};

}