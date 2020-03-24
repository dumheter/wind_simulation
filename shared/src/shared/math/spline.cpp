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

#include "spline.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "alflib/core/assert.hpp"

// ========================================================================== //
// Spline Implementation
// ========================================================================== //

namespace wind {

Spline::Spline(const std::vector<Vec3F> &points, u32 degree)
    : m_points(points), m_degree(degree) {
  AlfAssert(
      points.size() > degree,
      "Spline must consist of at least 'degree + 1' number of control points");

  // Initialize BSpline
  m_spline = ts_bspline_init();
  tsError error =
      ts_bspline_new(points.size(), 3, degree, TS_CLAMPED, &m_spline, nullptr);
  AlfAssert(error == TS_SUCCESS, "Failed to create spline");

  // Initialize DeBoor Net
  m_net = ts_deboornet_init();

  // Setup control points
  tsReal *_points;
  error = ts_bspline_control_points(&m_spline, &_points, nullptr);
  AlfAssert(error == TS_SUCCESS, "Failed to retrieve spline control points");
  for (size_t i = 0; i < points.size(); i++) {
    const Vec3F &point = points[i];
    const size_t off = i * 3;
    _points[off + 0] = point.x;
    _points[off + 1] = point.y;
    _points[off + 2] = point.z;
  }
  error = ts_bspline_set_control_points(&m_spline, _points, nullptr);
  AlfAssert(error == TS_SUCCESS, "Failed to set spline control points");
}

// -------------------------------------------------------------------------- //

Spline::~Spline() {
  ts_deboornet_free(&m_net);
  ts_bspline_free(&m_spline);
}

// -------------------------------------------------------------------------- //

void Spline::preSample(u32 samples) {
  const f32 len = calcLen(u32(getPoints().size()) * 10u);
  if (samples == kSplineSamplesAuto) {
    samples = u32(len);
  }

  m_sampleCache.clear();
  const f32 step = 1.0f / samples;
  for (u32 i = 1; i <= samples; i++) {
    const f32 t = step * i;
    m_sampleCache.push_back(sample(t));
  }
}

// -------------------------------------------------------------------------- //

Vec3F Spline::sample(f32 t) const {
  AlfAssert(t >= 0.0 && t <= 1.0, "t must be within the range [0.0, 1.0]");

  // Evaluate spline
  tsError error = ts_bspline_eval(&m_spline, t, &m_net, nullptr);
  AlfAssert(error == TS_SUCCESS, "Failed to evaluate spline");
  tsReal *result;

  // Retrieve result
  error = ts_deboornet_result(&m_net, &result, nullptr);
  AlfAssert(error == TS_SUCCESS, "Failed to evaluate spline");
  const f32 x = f32(result[0]);
  const f32 y = f32(result[1]);
  const f32 z = f32(result[2]);
  const Vec3F out(x, y, z);
  free(result);
  return out;
}

// -------------------------------------------------------------------------- //

f32 Spline::calcLenEst() const {
  const Vec3F &prev = m_points[0];
  f32 len = 0.0f;
  for (const Vec3F &point : m_points) {
    len += (point - prev).length();
  }
  return len;
}

// -------------------------------------------------------------------------- //

f32 Spline::calcLen(u32 samples) const {
  const Vec3F &prev = sample(0);
  const f32 delta = 1.0f / samples;
  f32 len = 0.0f;
  for (u32 i = 0; i < samples + 1; i++) {
    const f32 t = delta * i;
    len += (sample(t) - prev).length();
  }
  return len;
}

} // namespace wind
