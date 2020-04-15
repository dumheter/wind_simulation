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

#include "shared/scene/component/csim.hpp"
#include "shared/scene/component_handles.hpp"
#include "shared/sim/vector_field.hpp"

// ========================================================================== //
// DeltaField Declaration
// ========================================================================== //

namespace wind {

/// Delta field
class DeltaField {
public:
  /// Box plot data
  struct BoxPlot {
    f32 minVal, maxVal;         ///< Min and max values
    f32 minOutlier, maxOutlier; ///< Min and max outlier values
    f32 median;                 ///< Median (50th percentile)
    f32 perc25;                 ///< 25th percentile
    f32 perc75;                 ///< 25th percentile
  };

  /// Construct delta field
  DeltaField() = default;

  ~DeltaField();

  /// Build delta field by specifying a CSim component and CWind component
  void build(const HCSim &csim, const HCWind &cwind);

  /// Retrieve a delta vector
  Vec3F get(u32 x, u32 y, u32 z) const { return m_delta->get(x, y, z); }

  /// Check whether delta field is built
  bool isBuilt() const { return m_delta != nullptr; }

  f32 getError() const;

  BoxPlot boxPlot();

  /// Paint delta field
  void paint(Painter &painter, const Vec3F &offset = Vec3F::ZERO,
             const Vec3F &padding = Vec3F::ZERO) const;

  /// Set whether to draw the delta field
  void setDrawDelta(bool enabled) { m_drawDelta = enabled; }

  /// Set whether to draw the sim field
  void setDrawSim(bool enabled) { m_drawSim = enabled; }

  /// Set whether to draw the baked field
  void setDrawBaked(bool enabled) { m_drawBaked = enabled; }

private:
  /// Delta field
  VectorField *m_delta = nullptr;
  /// Sim field
  VectorField *m_sim = nullptr;
  /// Baked field
  VectorField *m_baked = nullptr;

  /// Whether to draw fields
  bool m_drawDelta = false, m_drawSim = false, m_drawBaked = false;
};

} // namespace wind
