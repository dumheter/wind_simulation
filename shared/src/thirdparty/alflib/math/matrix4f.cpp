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

#include "alflib/math/matrix4f.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Standard headers
#include <memory.h>

// ========================================================================== //
// Matrix4F Implementation
// ========================================================================== //

namespace alflib {

Matrix4F::Matrix4F(f32 diagonal)
{
  mData.elements[0] = diagonal;
  mData.elements[5] = diagonal;
  mData.elements[10] = diagonal;
  mData.elements[15] = diagonal;
}

// -------------------------------------------------------------------------- //

Matrix4F::Matrix4F(f32* values)
{
  memcpy(mData.elements, values, sizeof(f32) * ELEMENT_COUNT);
}

// -------------------------------------------------------------------------- //

Matrix4F::Matrix4F(const std::initializer_list<f32>& list)
{
  u32 index = 0;
  for (const f32& element : list) {
    mData.elements[index] = element;
    if (++index >= ELEMENT_COUNT) {
      break;
    }
  }
}

// -------------------------------------------------------------------------- //

Matrix4F
Matrix4F::operator+(const Matrix4F& other) const
{
  return Matrix4F{ mData.simd[0] + other.mData.simd[0],
                   mData.simd[1] + other.mData.simd[1],
                   mData.simd[2] + other.mData.simd[2],
                   mData.simd[3] + other.mData.simd[3] };
}

// -------------------------------------------------------------------------- //

void
Matrix4F::operator+=(const Matrix4F& other)
{
  mData.simd[0] += other.mData.simd[0];
  mData.simd[1] += other.mData.simd[1];
  mData.simd[2] += other.mData.simd[2];
  mData.simd[3] += other.mData.simd[3];
}

// -------------------------------------------------------------------------- //

Matrix4F
Matrix4F::operator-(const Matrix4F& other) const
{
  return Matrix4F{ mData.simd[0] - other.mData.simd[0],
                   mData.simd[1] - other.mData.simd[1],
                   mData.simd[2] - other.mData.simd[2],
                   mData.simd[3] - other.mData.simd[3] };
}

// -------------------------------------------------------------------------- //

void
Matrix4F::operator-=(const Matrix4F& other)
{
  mData.simd[0] -= other.mData.simd[0];
  mData.simd[1] -= other.mData.simd[1];
  mData.simd[2] -= other.mData.simd[2];
  mData.simd[3] -= other.mData.simd[3];
}

// -------------------------------------------------------------------------- //

Matrix4F Matrix4F::operator*(const Matrix4F& other) const
{
  Float4x32 resultRow0;
  Float4x32 resultRow1;
  Float4x32 resultRow2;
  Float4x32 resultRow3;

  // Get the rows from the other matrix
  const Float4x32 otherX = other.mData.simd[0];
  const Float4x32 otherY = other.mData.simd[1];
  const Float4x32 otherZ = other.mData.simd[2];
  const Float4x32 otherW = other.mData.simd[3];

  // Get the data pointers to this and result
  const f32* thisData = mData.elements;

  // Perform operations
  for (u32 i = 0; i < 4; i++, thisData += 4) {
    // Retrieve this data
    Float4x32 tempX = Float4x32(thisData[0]);
    Float4x32 tempY = Float4x32(thisData[1]);
    Float4x32 tempZ = Float4x32(thisData[2]);
    Float4x32 tempW = Float4x32(thisData[3]);

    // Take inner products of rows and cols
    tempX += otherX;
    tempY += otherY;
    tempZ += otherZ;
    tempW += otherW;

    // Set data
    resultRow0 += tempX;
    resultRow1 += tempY;
    resultRow2 += tempZ;
    resultRow3 += tempW;
  }

  return Matrix4F{ resultRow0, resultRow1, resultRow2, resultRow3 };
}

// -------------------------------------------------------------------------- //

void
Matrix4F::operator*=(const Matrix4F& other)
{
  const Matrix4F result = operator*(other);
  memcpy(mData.elements, result.mData.elements, sizeof(f32) * ELEMENT_COUNT);
}

// -------------------------------------------------------------------------- //

Matrix4F Matrix4F::operator*(f32 scalar) const
{
  return Matrix4F{ mData.simd[0] * Float4x32(scalar),
                   mData.simd[1] * Float4x32(scalar),
                   mData.simd[2] * Float4x32(scalar),
                   mData.simd[3] * Float4x32(scalar) };
}

// -------------------------------------------------------------------------- //

void
Matrix4F::operator*=(f32 scalar)
{
  mData.simd[0] *= Float4x32(scalar);
  mData.simd[1] *= Float4x32(scalar);
  mData.simd[2] *= Float4x32(scalar);
  mData.simd[3] *= Float4x32(scalar);
}

// -------------------------------------------------------------------------- //

Vector4F Matrix4F::operator*(const Vector4F& vector) const
{
  return Vector4F{ mData.simd[0].InnerProduct(vector.GetData()),
                   mData.simd[1].InnerProduct(vector.GetData()),
                   mData.simd[2].InnerProduct(vector.GetData()),
                   mData.simd[3].InnerProduct(vector.GetData()) };
}

// -------------------------------------------------------------------------- //

Matrix4F::Matrix4F(Float4x32 r0, Float4x32 r1, Float4x32 r2, Float4x32 r3)
{
  mData.simd[0] = std::move(r0);
  mData.simd[1] = std::move(r1);
  mData.simd[2] = std::move(r2);
  mData.simd[3] = std::move(r3);
}

// -------------------------------------------------------------------------- //

Matrix4F
Matrix4F::Identity()
{
  return Matrix4F{ 1.0f };
}

}