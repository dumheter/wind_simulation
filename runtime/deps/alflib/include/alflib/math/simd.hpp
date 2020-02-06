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

#include "alflib/platform/platform.hpp"

// ========================================================================== //
// Float4x32
// ========================================================================== //

namespace alflib {

/** \class Float4x32
 * \author Filip Björklund
 * \date 25 june 2019 - 13:27
 * \brief Simd 4x 32-bit float.
 * \details
 * Class that represents a SIMD enabled data that operates on 4 32-bit
 * floating-point numbers at the same time.
 */
class Float4x32
{
public:
  /** Number of components **/
  static constexpr u32 COMPONENT_COUNT = 4;

private:
  union Data
  {
#if defined(ALFLIB_ARCH_AMD64)
    /** x86 SIMD data **/
    __m128 simd;
#endif
    /** Separate data components **/
    struct Components
    {
      f32 x, y, z, w;
    } components;
    /** Array of data components **/
    f32 componentArray[4];
  } mData;

public:
  /** Construct from four (4) separate components.
   * \brief Construct from components.
   * \param x First component.
   * \param y Second component.
   * \param z Third component.
   * \param w Fourth component.
   */
  Float4x32(f32 x, f32 y, f32 z, f32 w);

  /** Construct from array of four (4) values.
   * \brief Construct from array.
   * \param values Array of values.
   */
  explicit Float4x32(f32 values[4]);

  /** Construct filled with the same value across each component.
   * \brief Construct filled with value.
   * \param value Value to fill with.
   */
  explicit Float4x32(f32 value = 0.0f);

  /** Construct from initializer list. Any components past the first four (4)
   * are ignored, and any missing of the first four (4) is substituted by 0.0.
   * \brief Construct from initializer list.
   * \param list Initializer list.
   */
  Float4x32(const std::initializer_list<f32>& list);

  /** Copy-constructor **/
  Float4x32(const Float4x32& other);

  /** Move-constructor **/
  Float4x32(Float4x32&& other) noexcept;

  /** Copy-assignment **/
  Float4x32& operator=(const Float4x32& other);

  /** Move-assignment **/
  Float4x32& operator=(Float4x32&& other) noexcept;

  /** Addition **/
  Float4x32 operator+(const Float4x32& other) const;

  /** Compound addition **/
  void operator+=(const Float4x32& other);

  /** Subtraction **/
  Float4x32 operator-(const Float4x32& other) const;

  /** Compound subtraction **/
  void operator-=(const Float4x32& other);

  /** Multiplication **/
  Float4x32 operator*(const Float4x32& other) const;

  /** Compound multiplication **/
  void operator*=(const Float4x32& other);

  /** Division **/
  Float4x32 operator/(const Float4x32& other) const;

  /** Compound division **/
  void operator/=(const Float4x32& other);

  /** Equality **/
  bool operator==(const Float4x32& other) const;

  /** Inequality **/
  bool operator!=(const Float4x32& other) const;

  /** Returns component at index 0-3 **/
  f32& operator[](u32 index);

  /** Returns component at index 0-3 **/
  const f32& operator[](u32 index) const;

  /** Returns inner product **/
  f32 InnerProduct(const Float4x32& other) const;

  /** Returns X component **/
  f32& GetX() { return mData.components.x; }

  /** Returns X component **/
  const f32& GetX() const { return mData.components.x; }

  /** Returns Y component **/
  f32& GetY() { return mData.components.y; }

  /** Returns Y component **/
  const f32& GetY() const { return mData.components.y; }

  /** Returns Z component **/
  f32& GetZ() { return mData.components.z; }

  /** Returns Z component **/
  const f32& GetZ() const { return mData.components.z; }

  /** Returns W component **/
  f32& GetW() { return mData.components.w; }

  /** Returns W component **/
  const f32& GetW() const { return mData.components.w; }

private:
#if defined(ALFLIB_ARCH_AMD64)
  /** Construct from simd data **/
  Float4x32(__m128 simd);
#endif
};

}

// ========================================================================== //
// Float2x32 Declaration
// ========================================================================== //

namespace alflib {

/** \class Float2x32
 * \author Filip Björklund
 * \date 25 june 2019 - 13:35
 * \brief Simd 2x 32-bit float.
 * \details
 * Class that represents a SIMD enabled data that operates on 2 32-bit
 * floating-point numbers at the same time.
 */
class Float2x32
{
public:
  /** Number of components **/
  static constexpr u32 COMPONENT_COUNT = 2;

private:
  union Data
  {
#if defined(ALFLIB_ARCH_AMD64)
    /** x86 SIMD data **/
    __m64 simd;
#endif
    /** Separate data components **/
    struct Components
    {
      f32 x, y;
    } components;
    /** Array of data components **/
    f32 componentArray[2];
  } mData;

public:
  /** Construct from two (2) separate components.
   * \brief Construct from components.
   * \param x First component.
   * \param y Second component.
   */
  Float2x32(f32 x, f32 y);

  /** Construct from array of two (2) values.
   * \brief Construct from array.
   * \param values Array of values.
   */
  explicit Float2x32(f32 values[2]);

  /** Construct filled with the same value across each component.
   * \brief Construct filled with value.
   * \param value Value to fill with.
   */
  explicit Float2x32(f32 value = 0.0f);

  /** Construct from initializer list. Any components past the first two (2)
   * are ignored, and any missing of the first two (2) is substituted by 0.0.
   * \brief Construct from initializer list.
   * \param list Initializer list.
   */
  Float2x32(const std::initializer_list<f32>& list);

  /** Copy-constructor **/
  Float2x32(const Float2x32& other);

  /** Move-constructor **/
  Float2x32(Float2x32&& other) noexcept;

  /** Copy-assignment **/
  Float2x32& operator=(const Float2x32& other);

  /** Move-assignment **/
  Float2x32& operator=(Float2x32&& other) noexcept;

  /** Addition **/
  Float2x32 operator+(const Float2x32& other) const;

  /** Compound addition **/
  void operator+=(const Float2x32& other);

  /** Subtraction **/
  Float2x32 operator-(const Float2x32& other) const;

  /** Compound subtraction **/
  void operator-=(const Float2x32& other);

  /** Multiplication **/
  Float2x32 operator*(const Float2x32& other) const;

  /** Compound multiplication **/
  void operator*=(const Float2x32& other);

  /** Division **/
  Float2x32 operator/(const Float2x32& other) const;

  /** Compound division **/
  void operator/=(const Float2x32& other);

  /** Equality **/
  bool operator==(const Float2x32& other) const;

  /** Inequality **/
  bool operator!=(const Float2x32& other) const;

  /** Returns component at index 0-3 **/
  f32& operator[](u32 index);

  /** Returns component at index 0-3 **/
  const f32& operator[](u32 index) const;

  /** Returns inner product **/
  f32 InnerProduct(const Float2x32& other) const;

  /** Returns X component **/
  f32& GetX() { return mData.components.x; }

  /** Returns X component **/
  const f32& GetX() const { return mData.components.x; }

  /** Returns Y component **/
  f32& GetY() { return mData.components.y; }

  /** Returns Y component **/
  const f32& GetY() const { return mData.components.y; }

private:
#if defined(ALFLIB_ARCH_AMD64)
  /** Construct from simd data **/
  Float2x32(__m64 simd);

  /** Construct from simd data **/
  Float2x32(__m128 simd);

  /** Load data to __m128 **/
  __m128 LoadSimd128() const;

  /** Store data from __m128 **/
  void StoreSimd128(__m128 data);
#endif
};

}