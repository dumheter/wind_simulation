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
#include "alflib/math/vector4f.hpp"

// ========================================================================== //
// Matrix4F Declaration
// ========================================================================== //

namespace alflib {

/** \class Matrix4F
 * \author Filip Björklund
 * \date 25 june 2019 - 15:27
 * \brief
 * \details
 *
 */
class Matrix4F
{
public:
  /** Number of elements in matrix **/
  static constexpr u32 ELEMENT_COUNT = 16;

private:
  union Data
  {
    /** Simd data **/
    Float4x32 simd[4] = { Float4x32{}, Float4x32{}, Float4x32{}, Float4x32{} };
    /** Array data **/
    f32 elements[16];
  } mData{};

public:
  /** Construct matrix with specified value in diagonal cells **/
  explicit Matrix4F(f32 diagonal = 0.0f);

  /** Construct matrix from list of values. Pointer is expected to point to an
   * array of at least 16 f32 values **/
  explicit Matrix4F(f32* values);

  /** Construct matrix from initializer list. If list is shorter than 16 then
   * zeroes are used to pad **/
  Matrix4F(const std::initializer_list<f32>& list);

  /** Addition **/
  Matrix4F operator+(const Matrix4F& other) const;

  /** Compound addition **/
  void operator+=(const Matrix4F& other);

  /** Subtraction **/
  Matrix4F operator-(const Matrix4F& other) const;

  /** Compound subtraction **/
  void operator-=(const Matrix4F& other);

  /** Multiplication **/
  Matrix4F operator*(const Matrix4F& other) const;

  /** Compound multiplication **/
  void operator*=(const Matrix4F& other);

  /** Matrix-Scalar multiplication **/
  Matrix4F operator*(f32 scalar) const;

  /** Matrix-Scalar multiplication **/
  void operator*=(f32 scalar);

  /** Matrix-Vector multiplication **/
  Vector4F operator*(const Vector4F& vector) const;

private:
  /** Construct matrix from 4 simd values for rows **/
  Matrix4F(Float4x32 r0, Float4x32 r1, Float4x32 r2, Float4x32 r3);

public:
  /** Returns an identity matrix **/
  static Matrix4F Identity();
};

}
