// MIT License
//
// Copyright (c) 2019 Filip Bj�rklund
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
#include "alflib/core/string.hpp"

#include "alflib/platform/detection.hpp"

// ========================================================================== //
// Macros
// ========================================================================== //

/** Macro for a asserting a condition. This does not depend on any other classes
 * in the library to function **/
#define AlfBasicAssert(condition, message) assert(!!(condition) && message)

#if defined(ALFLIB_TARGET_WINDOWS)
/** Macro for asserting that a condition is true **/
#define AlfAssert(condition, messageFormat, ...)                               \
  alflib::Assert(condition, __FILE__, __LINE__, messageFormat, ##__VA_ARGS__)

#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
/** Macro for asserting that a condition is true **/
#define AlfAssert(condition, messageFormat, ...)                               \
  alflib::Assert(!!(condition), __FILE__, __LINE__, messageFormat, ##__VA_ARGS__)
#pragma GCC diagnostic pop
#endif

// ========================================================================== //
// Functions
// ========================================================================== //

namespace alflib {

/** Assert that the condition is true. If the condition is false a message will
 * be displayed and the execution will stop.
 * \brief Assert condition.
 * \param condition Condition to assert true.
 * \param file File in which assert occured.
 * \param line Line inside file where assert occured.
 * \param message Message that is displayed.
 */
void
Assert(bool condition, const char8* file, u32 line, const String& message);

// -------------------------------------------------------------------------- //

/** Assert that the condition is true. If the condition is false a message will
 * be displayed and the execution will stop. This function takes a format string
 * and arguments that are used to format the actual message.
 * \note Formatting is done according to the formatting rules of a
 * alflib::String.
 * \brief Assert condition.
 * \tparam ARGS Type of arguments to formatting.
 * \param condition Condition to assert true.
 * \param file File in which assert occured.
 * \param line Line inside file where assert occured.
 * \param format Format string.
 * \param arguments Arguments to format string.
 */
template<typename... ARGS>
void
Assert(bool condition,
       const char8* file,
       u32 line,
       const String& format,
       ARGS&&... arguments)
{
  Assert(condition,
         file,
         line,
         String(format).Format(std::forward<ARGS>(arguments)...));
}

}
