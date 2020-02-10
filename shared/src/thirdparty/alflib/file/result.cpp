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

#include "alflib/file/result.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace alflib {

std::ostream&
operator<<(std::ostream& os, const FileResult& result)
{
  switch (result) {
    case FileResult::kSuccess: {
      os << "kSuccess";
      break;
    }
    case FileResult::kUnknownError: {
      os << "kUnknownError";
      break;
    }
    case FileResult::kInvalidArgument: {
      os << "kInvalidArgument";
      break;
    }
    case FileResult::kOutOfMemory: {
      os << "kOutOfMemory";
      break;
    }
    case FileResult::kNotOpen: {
      os << "kNotOpen";
      break;
    }
    case FileResult::kAlreadyOpen: {
      os << "kAlreadyOpen";
      break;
    }
    case FileResult::kNotFound: {
      os << "kNotFound";
      break;
    }
    case FileResult::kAlreadyExists: {
      os << "kAlreadyExists";
      break;
    }
    case FileResult::kAccessDenied: {
      os << "kAccessDenied";
      break;
    }
    case FileResult::kEOF: {
      os << "kEOF";
      break;
    }
  }
  return os;
}

}