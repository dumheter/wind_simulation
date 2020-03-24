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

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <shared/math/math.hpp>

#include <tinyspline/tinyspline.h>

// ========================================================================== //
// Spline Declaration
// ========================================================================== //

namespace wind {

/// B-spline
class Spline {

public:
  /// Invalid number of samples. Used as default
  static constexpr u32 kSplineSamplesAuto = std::numeric_limits<u32>::max();

  /// Construct a spline from a list of points and an optional list of knots.
  explicit Spline(const std::vector<Vec3F> &points, u32 degree = 2);

  /// Destruct spline
  ~Spline();

  /// Pre-sample spline and cache result
  void preSample(u32 samples = kSplineSamplesAuto);

  /// Sample the spline at time 't' (0.0 -> 1.0).
  Vec3F sample(f32 t) const;

  /// Returns the points of the spline.
  const std::vector<Vec3F> &getPoints() const { return m_points; }

  /// Returns pre-sampled points
  const std::vector<Vec3F> &getPreSampledPoints() const {
    return m_sampleCache;
  }

  /// Returns whether or not the spline is pre-sampled
  bool isPreSampled() const { return !m_sampleCache.empty(); }

  /// Returns the degree of the spline.
  u32 getDegree() const { return m_degree; }

  /// Calculate an estimated length of the spline
  f32 calcLenEst() const;

  /// Calculate a length from an interpolated spline with N samples
  f32 calcLen(u32 samples) const;

private:
  /// Control points
  std::vector<Vec3F> m_points;
  /// Degree of spline
  u32 m_degree;

  /// Cache with pre-sampled points
  std::vector<Vec3F> m_sampleCache;

  /// Spline handle
  tsBSpline m_spline = {};
  /// DeBoor net
  mutable tsDeBoorNet m_net = {};
};

} // namespace wind