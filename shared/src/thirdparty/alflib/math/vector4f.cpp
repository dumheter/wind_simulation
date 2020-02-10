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

#include "alflib/math/vector4f.hpp"

// ========================================================================== //
// Vector4F Implementation
// ========================================================================== //

namespace alflib {

Vector4F::Vector4F(f32 x, f32 y, f32 z, f32 w)
  : mData(x, y, z, w)
{}

// -------------------------------------------------------------------------- //

Vector4F::Vector4F(f32 value)
  : mData(value)
{
  mData[3] = 1.0f;
}

// -------------------------------------------------------------------------- //

Vector4F
Vector4F::operator+(const Vector4F& other)
{
  return Vector4F{ mData + other.mData };
}

// -------------------------------------------------------------------------- //

void
Vector4F::operator+=(const Vector4F& other)
{
  mData += other.mData;
}

// -------------------------------------------------------------------------- //

Vector4F
Vector4F::operator-(const Vector4F& other)
{
  return Vector4F{ mData - other.mData };
}

// -------------------------------------------------------------------------- //

void
Vector4F::operator-=(const Vector4F& other)
{
  mData -= other.mData;
}

// -------------------------------------------------------------------------- //

Vector4F Vector4F::operator*(const Vector4F& other)
{
  return Vector4F{ mData * other.mData };
}

// -------------------------------------------------------------------------- //

void
Vector4F::operator*=(const Vector4F& other)
{
  mData *= other.mData;
}

// -------------------------------------------------------------------------- //

Vector4F
Vector4F::operator/(const Vector4F& other)
{
  return Vector4F{ mData / other.mData };
}

// -------------------------------------------------------------------------- //

void
Vector4F::operator/=(const Vector4F& other)
{
  mData /= other.mData;
}

// -------------------------------------------------------------------------- //

bool
Vector4F::operator==(const Vector4F& other)
{
  return mData == other.mData;
}

// -------------------------------------------------------------------------- //

bool
Vector4F::operator!=(const Vector4F& other)
{
  return mData != other.mData;
}

// -------------------------------------------------------------------------- //

f32& Vector4F::operator[](u32 index)
{
  return mData[index];
}

// -------------------------------------------------------------------------- //

const f32& Vector4F::operator[](u32 index) const
{
  return mData[index];
}

// -------------------------------------------------------------------------- //

f32
Vector4F::Dot(const Vector4F& other) const
{
  return mData.InnerProduct(other.mData);
}

// -------------------------------------------------------------------------- //

Vector4F::Vector4F(const Float4x32& data)
  : mData(data)
{}

}