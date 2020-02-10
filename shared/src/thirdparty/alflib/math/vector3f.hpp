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
// Vector3F Declaration
// ========================================================================== //

namespace alflib {

/** \class Vector4F
 * \author Filip Björklund
 * \date 25 june 2019 - 14:51
 * \brief 3D Vector.
 * \details
 * Class that represents a vector with 3 dimensions.
 *
 * It should be noted that this vector is still implemented using underlying
 * 4x32 simd data. Meaning it occupies 16 bytes in memory just like the
 * Vector4F.
 */
class Vector3F
{
private:
  /** Simd data **/
  Float4x32 mData;

public:
  /** Construct from values **/
  Vector3F(f32 x, f32 y, f32 z);

  /** Construct and fill with a single value **/
  explicit Vector3F(f32 value = 0.0f);

  /** Addition **/
  Vector3F operator+(const Vector3F& other);

  /** Compound addition **/
  void operator+=(const Vector3F& other);

  /** Subtraction **/
  Vector3F operator-(const Vector3F& other);

  /** Compound subtraction **/
  void operator-=(const Vector3F& other);

  /** Multiplication **/
  Vector3F operator*(const Vector3F& other);

  /** Compound multiplication **/
  void operator*=(const Vector3F& other);

  /** Division **/
  Vector3F operator/(const Vector3F& other);

  /** Compound division **/
  void operator/=(const Vector3F& other);

  /** Equality **/
  bool operator==(const Vector3F& other);

  /** Inequality **/
  bool operator!=(const Vector3F& other);

  /** Returns value and index **/
  f32& operator[](u32 index);

  /** Returns value and index **/
  const f32& operator[](u32 index) const;

  /** Dot product **/
  f32 Dot(const Vector3F& other) const;

  /** Cross product **/
  Vector3F Cross(const Vector3F& other);

  /** Returns length of vector **/
  f32 Length() const;

  /** Normalizes vector **/
  void Normalize();

private:
  /** Construct from data **/
  Vector3F(const Float4x32& data);
};

}
