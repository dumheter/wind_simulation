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

// Project headers
#include "alflib/math/simd.hpp"

// ========================================================================== //
// Vector2F Declaration
// ========================================================================== //

namespace alflib {

/** \class Vector2F
 * \author Filip Björklund
 * \date 25 june 2019 - 15:23
 * \brief 2D Vector.
 * \details
 * Class that represents a vector with 2 dimensions.
 */
class Vector2F
{
private:
  /** Simd data **/
  Float2x32 mData;

public:
  /** Construct from values **/
  Vector2F(f32 x, f32 y);

  /** Construct and fill with a single value **/
  explicit Vector2F(f32 value = 0.0f);

  /** Addition **/
  Vector2F operator+(const Vector2F& other) const;

  /** Compound addition **/
  void operator+=(const Vector2F& other);

  /** Subtraction **/
  Vector2F operator-(const Vector2F& other) const;

  /** Compound subtraction **/
  void operator-=(const Vector2F& other);

  /** Multiplication **/
  Vector2F operator*(const Vector2F& other) const;

  /** Compound multiplication **/
  void operator*=(const Vector2F& other);

  /** Division **/
  Vector2F operator/(const Vector2F& other) const;

  /** Compound division **/
  void operator/=(const Vector2F& other);

  /** Equality **/
  bool operator==(const Vector2F& other) const;

  /** Inequality **/
  bool operator!=(const Vector2F& other) const;

  /** Returns value and index **/
  f32& operator[](u32 index);

  /** Returns value and index **/
  const f32& operator[](u32 index) const;

  /** Dot product **/
  f32 Dot(const Vector2F& other) const;

private:
  /** Construct from data **/
  explicit Vector2F(const Float2x32& data);
};

}
