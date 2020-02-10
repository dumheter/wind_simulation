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
#include "alflib/core/common.hpp"
#include "alflib/core/traits.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace alflib {

/** Swap endian of a 16-bit value.
 * \brief Swap 16-bit endian.
 * \param value Value to swap endian of.
 * \return Value with swapped endian.
 */
constexpr u16
SwapEndian(u16 value)
{
  return ((value & 0xff00u) >> 8u) | ((value & 0x00ffu) << 8u);
}

// -------------------------------------------------------------------------- //

/** Swap endian of a 32-bit value.
 * \brief Swap 32-bit endian.
 * \param value Value to swap endian of.
 * \return Value with swapped endian.
 */
constexpr u32
SwapEndian(u32 value)
{
  return ((value & 0xff000000u) >> 24u) | ((value & 0x00ff0000u) >> 8u) |
         ((value & 0x0000ff00u) << 8u) | ((value & 0x000000ffu) << 24u);
}

// -------------------------------------------------------------------------- //

/** Swap endian of a 64-bit value.
 * \brief Swap 64-bit endian.
 * \param value Value to swap endian of.
 * \return Value with swapped endian.
 */
constexpr u64
SwapEndian(u64 value)
{
  return ((value & 0xff00000000000000ull) >> 56u) |
         ((value & 0x00ff000000000000ull) >> 40u) |
         ((value & 0x0000ff0000000000ull) >> 24u) |
         ((value & 0x000000ff00000000ull) >> 8u) |
         ((value & 0x00000000ff000000ull) << 8u) |
         ((value & 0x0000000000ff0000ull) << 24u) |
         ((value & 0x000000000000ff00ull) << 40u) |
         ((value & 0x00000000000000ffull) << 56u);
}

// -------------------------------------------------------------------------- //

/** Relocate memory from the source memory location to the destination location.
 * If the type is trivially relocatable then this will copy the memory using
 * 'memcpy' instead of invoking the move constructor and then destruct the
 * source value.
 * \brief Relocate memory.
 * \tparam T Type of object to relocate memory of.
 * \param destination Destination address.
 * \param source Source address.
 * \return Destination address.
 */
template<typename T>
T*
Relocate(T* destination, T* source)
{
  if constexpr (!IsTriviallyRelocatable<T>::Value) {
    ::new (destination) T{ std::move(*source) };
    source->~T();
  } else {
    memcpy(destination, source, sizeof(T));
  }
  return destination;
}

}