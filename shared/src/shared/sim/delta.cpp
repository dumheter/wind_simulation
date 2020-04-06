// MIT License
//
// Copyright (c) 2020 Filip Bj�rklund�, Christoffer Gustafsson
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

void DeltaField::build(const HCSim &csim, const HCWind &cwind) {
  WindSimulation *sim = csim->getSim();
  const FieldBase::Dim dim = sim->getDim();
  m_delta =
      new VectorField(dim.width, dim.height, dim.depth, sim->getCellSize());

  // Construct delta
  for (u32 k = 0; k < dim.depth; k++) {
    for (u32 j = 0; j < dim.height; j++) {
      for (u32 i = 0; i < dim.width; i++) {
        if (sim->O().get(i, j, k)) {
          m_delta->set(i, j, k, Vec3F::ZERO);
        } else {
          Vec3F vSim = sim->V().get(i, j, k);
          Vec3F vBake = cwind->getWindAtPoint(Vec3F(i, j, k));
          m_delta->set(i, j, k, vBake - vSim);
        }
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void DeltaField::paint(Painter &painter, const Vec3F &offset,
                       const Vec3F &padding) const {
  m_delta->paint(painter, offset, padding);
}

} // namespace wind
