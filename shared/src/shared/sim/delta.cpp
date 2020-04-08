// MIT License
//
// Copyright (c) 2020 Filip Björklund¨, Christoffer Gustafsson
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

#include "delta.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/scene/component/cwind.hpp"

// ========================================================================== //
// DeltaField Implementation
// ========================================================================== //

namespace wind {

DeltaField::~DeltaField() {
  delete m_delta;
  delete m_sim;
  delete m_baked;
}

// -------------------------------------------------------------------------- //

void DeltaField::build(const HCSim &csim, const HCWind &cwind) {
  WindSimulation *sim = csim->getSim();
  const FieldBase::Dim dim = sim->getDim();
  m_delta =
      new VectorField(dim.width, dim.height, dim.depth, sim->getCellSize());
  m_sim = new VectorField(dim.width, dim.height, dim.depth, sim->getCellSize());
  m_baked =
      new VectorField(dim.width, dim.height, dim.depth, sim->getCellSize());

  // Construct delta
  for (u32 k = 0; k < dim.depth; k++) {
    for (u32 j = 0; j < dim.height; j++) {
      for (u32 i = 0; i < dim.width; i++) {
        Vec3F vSim = sim->V().get(i, j, k);
        Vec3F vBake = cwind->getWindAtPoint(
            Vec3F(i + 0.5f, j + 0.5f, k + 0.5f) * sim->getCellSize());
        const bool obs = sim->O().get(i, j, k);
        m_delta->set(i, j, k, obs ? Vec3F::ZERO : vBake - vSim);
        m_sim->set(i, j, k, vSim);
        m_baked->set(i, j, k, vBake);
      }
    }
  }
}

// -------------------------------------------------------------------------- //

f32 DeltaField::getError() const {
  const FieldBase::Dim dim = m_delta->getDim();
  const u32 count = dim.width * dim.height * dim.depth;
  f32 error = 0.0f;
  for (u32 k = 0; k < dim.depth; k++) {
    for (u32 j = 0; j < dim.height; j++) {
      for (u32 i = 0; i < dim.width; i++) {
        error += m_delta->get(i, j, k).length();
      }
    }
  }
  return error / count;
}

// -------------------------------------------------------------------------- //

void DeltaField::paint(Painter &painter, const Vec3F &offset,
                       const Vec3F &padding) const {
  if (m_drawDelta) {
    m_delta->paintWithColor(painter, Color::green(), offset, padding);
  }
  if (m_drawSim) {
    m_sim->paintWithColor(painter, Color::red(), offset, padding);
  }
  if (m_drawBaked) {
    m_baked->paintWithColor(painter, Color::yellow(), offset, padding);
  }
}

} // namespace wind
