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

// ========================================================================== //
// IsTriviallyRelocatable
// ========================================================================== //

#define ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(type)                              \
  template<>                                                                   \
  struct IsTriviallyRelocatable<type>                                          \
  {                                                                            \
    static constexpr bool Value = true;                                        \
  };

namespace alflib {

/** Trait that determines whether or not a type is trivially relocatable.
 * Meaning that we don't need to use a 'Move-Destruct' pair when moving the data
 * of the object, but can rather just use a 'memcpy'.
 * \brief Trait for trivially relocatable types.
 * \tparam T Type that may be trivially relocatable.
 */
template<typename T>
struct IsTriviallyRelocatable
{
  static constexpr bool Value = false;
};

// -------------------------------------------------------------------------- //

/** \copydoc alflib::IsTriviallyRelocatable<T> **/
template<typename T>
struct IsTriviallyRelocatable<T*>
{
  static constexpr bool Value = true;
};

// -------------------------------------------------------------------------- //

ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(char);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(unsigned char);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(short);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(unsigned short);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(int);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(unsigned int);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(long);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(unsigned long);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(long long);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(unsigned long long);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(float);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(double);
ALFLIB_DEFINE_TRIVIALLY_RELOCATABLE(long double);

}