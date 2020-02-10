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

#include "alflib/core/string.hpp"

// ========================================================================== //
// Console Declaration
// ========================================================================== //

namespace alflib {

/** \class
 * \author Filip Björklund
 * \date 01 januari 2019 - 00:00
 * \brief Console iteractions.
 * \details
 * Class that represents a way to access and interact with the standard output.
 */
class Console
{
public:
  /** Write a formatted message to the standard output.
   * \note Message is formatted according to the rules of the 'fmt' library.
   * \brief Write to stdout.
   * \tparam ARGS Argument types.
   * \param format Message format string.
   * \param arguments Arguments to format string with.
   */
  template<typename... ARGS>
  static void Write(const String& format, ARGS&&... arguments);

  /** Write a formatted message to the standard output followed by a newline.
   * \note Message is formatted according to the rules of the 'fmt' library.
   * \brief Write to stdout.
   * \tparam ARGS Argument types.
   * \param format Message format string.
   * \param arguments Arguments to format string with.
   */
  template<typename... ARGS>
  static void WriteLine(const String& format, ARGS&&... arguments);

private:
  /** Write to standard output **/
  static void Write_(const String& message);

  /** Write a line to standard output (newline is appended) **/
  static void WriteLine_(const String& message);
};

// -------------------------------------------------------------------------- //

template<typename... ARGS>
void
Console::Write(const String& format, ARGS&&... arguments)
{
  Write_(format.Format(std::forward<ARGS>(arguments)...));
}

// -------------------------------------------------------------------------- //

template<typename... ARGS>
void
Console::WriteLine(const String& format, ARGS&&... arguments)
{
  WriteLine_(format.Format(std::forward<ARGS>(arguments)...));
}

}