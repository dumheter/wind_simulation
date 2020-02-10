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

#include "alflib/math/vector3f.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Project headers
#include "alflib/math/math.hpp"

// ========================================================================== //
// Vector3F Implementation
// ========================================================================== //

namespace alflib {

Vector3F::Vector3F(f32 x, f32 y, f32 z)
  : mData(x, y, z, 0.0f)
{}

// -------------------------------------------------------------------------- //

Vector3F::Vector3F(f32 value)
  : mData(value)
{
  mData[3] = 1.0f;
}

// -------------------------------------------------------------------------- //

Vector3F
Vector3F::operator+(const Vector3F& other)
{
  return Vector3F{ mData + other.mData };
}

// -------------------------------------------------------------------------- //

void
Vector3F::operator+=(const Vector3F& other)
{
  mData += other.mData;
}

// -------------------------------------------------------------------------- //

Vector3F
Vector3F::operator-(const Vector3F& other)
{
  return Vector3F{ mData - other.mData };
}

// -------------------------------------------------------------------------- //

void
Vector3F::operator-=(const Vector3F& other)
{
  mData -= other.mData;
}

// -------------------------------------------------------------------------- //

Vector3F Vector3F::operator*(const Vector3F& other)
{
  return Vector3F{ mData * other.mData };
}

// -------------------------------------------------------------------------- //

void
Vector3F::operator*=(const Vector3F& other)
{
  mData *= other.mData;
}

// -------------------------------------------------------------------------- //

Vector3F
Vector3F::operator/(const Vector3F& other)
{
  return Vector3F{ mData / other.mData };
}

// -------------------------------------------------------------------------- //

void
Vector3F::operator/=(const Vector3F& other)
{
  mData /= other.mData;
}

// -------------------------------------------------------------------------- //

bool
Vector3F::operator==(const Vector3F& other)
{
  return mData == other.mData;
}

// -------------------------------------------------------------------------- //

bool
Vector3F::operator!=(const Vector3F& other)
{
  return mData != other.mData;
}

// -------------------------------------------------------------------------- //

f32& Vector3F::operator[](u32 index)
{
  return mData[index];
}

// -------------------------------------------------------------------------- //

const f32& Vector3F::operator[](u32 index) const
{
  return mData[index];
}

// -------------------------------------------------------------------------- //

f32
Vector3F::Dot(const Vector3F& other) const
{
  return mData.InnerProduct(other.mData);
}

// -------------------------------------------------------------------------- //

Vector3F
Vector3F::Cross(const Vector3F& other)
{
  return Vector3F{ mData[1] * other.mData[2] - mData[2] * other.mData[1],
                   mData[0] * other.mData[2] - mData[2] * other.mData[0],
                   mData[0] * other.mData[1] - mData[1] * other.mData[0] };
}

// -------------------------------------------------------------------------- //

f32
Vector3F::Length() const
{
  return SquareRoot(mData.GetX() * mData.GetX() + mData.GetY() * mData.GetY() +
                    mData.GetZ() * mData.GetZ());
}

// -------------------------------------------------------------------------- //

void
Vector3F::Normalize()
{
  const f32 length = Length();
  mData.GetX() /= length;
  mData.GetY() /= length;
  mData.GetZ() /= length;
}

// -------------------------------------------------------------------------- //

Vector3F::Vector3F(const Float4x32& data)
  : mData(data)
{}

}