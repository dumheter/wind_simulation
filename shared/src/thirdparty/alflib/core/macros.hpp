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
// Macro Declarations
// ========================================================================== //

/** Macro for forward-declaring a simple class **/
#define ALFLIB_FORWARD_DECLARE_CLASS(cls) class cls;

// -------------------------------------------------------------------------- //

/** Macro for forward-declaring a simple class **/
#define ALFLIB_FORWARD_DECLARE_ENUM_CLASS(cls, type) enum class cls : type;

// -------------------------------------------------------------------------- //

/** Macro for generating operators for an enum class **/
#define ALFLIB_ENUM_CLASS_OPERATORS(prefix, type, underlying_type)             \
  prefix type operator|(type lhs, type rhs)                                    \
  {                                                                            \
    return static_cast<type>(static_cast<underlying_type>(lhs) |               \
                             static_cast<underlying_type>(rhs));               \
  }                                                                            \
  prefix type operator&(type lhs, type rhs)                                    \
  {                                                                            \
    return static_cast<type>(static_cast<underlying_type>(lhs) &               \
                             static_cast<underlying_type>(rhs));               \
  }                                                                            \
  prefix type& operator|=(type& lhs, type rhs)                                 \
  {                                                                            \
    lhs = (lhs | rhs);                                                         \
    return lhs;                                                                \
  }

// -------------------------------------------------------------------------- //

/** Macro for declaring a class non-copyable **/
#define ALFLIB_CLASS_NON_COPYABLE(type)                                        \
  type(const type&) = delete;                                                  \
  type& operator=(const type&) = delete;

// -------------------------------------------------------------------------- //

/** Macro for declaring a class default-movable **/
#define ALFLIB_CLASS_DEFAULT_MOVABLE(type)                                     \
  type(type&&) = default;                                                      \
  type& operator=(type&&) = default;
