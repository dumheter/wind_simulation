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

#include "alflib/core/assert.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Project headers
#include "alflib/core/dialog.hpp"
#include "alflib/core/console.hpp"
#include "alflib/debug/debugger.hpp"
#include <iostream>

// ========================================================================== //
// Functions
// ========================================================================== //

namespace alflib {

void
Assert(bool condition, const char8* file, u32 line, const String& message)
{
  // Return if the condition is true (holds)
  if (condition) {
    return;
  }

  // Display message and abort
  String output =
    String("{}:{}: Failed with message: '{}'").Format(file, line, message);

  // Print to stdout and show dialog
  Console::WriteLine(output);
  std::cout << std::endl; // flushes the output
  ShowErrorDialog("Assertion", output);

  // Break debugger then abort execution
  BreakDebugger();
  exit(-1);
}

}
