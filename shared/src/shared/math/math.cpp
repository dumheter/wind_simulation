// MIT License
//
// Copyright (c) 2020 Filip Björklund, Christoffer Gustafsson
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

#include "math.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <dlog/dlog.hpp>

// ========================================================================== //
// Functions
// ========================================================================== //

namespace wind {

f32 gaussian(f32 x, f32 scalar, f32 offset, f32 width) {
  // const f32 safeWidth = std::max(1.0f, width);
  const f32 safeWidth = width;
  return scalar *
         std::exp(-((x - offset) * (x - offset)) / (2 * safeWidth * safeWidth));
}

// -------------------------------------------------------------------------- //

void medianIndices(size_t length, size_t &lower, size_t &upper) {
  lower = size_t(ceil(length / 2.0f) - 1);
  upper = size_t(floor(length / 2.0f));
}

// -------------------------------------------------------------------------- //

f32 median(const std::vector<f32> &values, size_t left, size_t right) {
  AlfAssert(right >= left, "Max index cannot exceed min index");
  const size_t count = right - left;

  // Base cases
  if (count == 0) {
    return 0.0f;
  }
  if (count < 2) {
    return values[left];
  }

  // General case
  size_t lower_, upper_;
  medianIndices(count, lower_, upper_);
  return (values[lower_ + left] + values[upper_ + left]) / 2.0f;
}

// -------------------------------------------------------------------------- //

f32 median(const std::vector<f32> &values) {
  return median(values, 0, values.size());
}

// -------------------------------------------------------------------------- //

f32 quartile1(const std::vector<f32> &values) {
  size_t lower, upper;
  medianIndices(values.size(), lower, upper);
  return median(values, 0, lower + 1);
}

// -------------------------------------------------------------------------- //

f32 quartile3(const std::vector<f32> &values) {
  size_t lower, upper;
  medianIndices(values.size(), lower, upper);
  return median(values, upper, values.size());
}

} // namespace wind
