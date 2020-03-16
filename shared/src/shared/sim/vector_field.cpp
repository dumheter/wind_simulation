// MIT License
//
// Copyright (c) 2020 Filip Bj�rklund, Christoffer Gustafsson
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

#include "shared/sim/vector_field.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/math/math.hpp"
#include "shared/render/painter.hpp"

#include <Debug/BsDebugDraw.h>

#include "shared/debug/debug_manager.hpp"

// ========================================================================== //
// VectorField Implementation
// ========================================================================== //

namespace wind {

VectorField::VectorField(u32 width, u32 height, u32 depth, f32 cellSize) {
  using namespace bs;

  m_x = new Comp(width, height, depth, cellSize);
  m_y = new Comp(width, height, depth, cellSize);
  m_z = new Comp(width, height, depth, cellSize);
  m_dim = m_x->getDim();
  m_cellSize = m_x->getCellSize();
}

// -------------------------------------------------------------------------- //

void VectorField::paint(Painter &painter, const Vec3F &offset,
                        ObstructionField *obstructionField,
                        const Vec3F &padding) {
  bs::Vector<bs::Vector3> linesRed;
  bs::Vector<bs::Vector3> linesYellow;

  const u32 xPad = u32(padding.x);
  const u32 yPad = u32(padding.y);
  const u32 zPad = u32(padding.z);

  // Draw vectors
  for (u32 z = zPad; z < m_dim.depth - zPad; z++) {
    const f32 zPos = offset.z + (z * m_cellSize);
    for (u32 y = yPad; y < m_dim.height - yPad; y++) {
      const f32 yPos = offset.y + (y * m_cellSize);
      for (u32 x = xPad; x < m_dim.width - xPad; x++) {
        const f32 xPos = offset.x + (x * m_cellSize);
        const Vec3F base =
            Vec3F(xPos + (m_cellSize / 2.0f), yPos + (m_cellSize / 2.0f),
                  zPos + (m_cellSize / 2.0f)) -
            (padding * m_cellSize);
        const bs::Vector3 &vec = get(x, y, z);
        if (obstructionField->get(x, y, z)) {
          Painter::buildArrow(linesYellow, base, vec, 0.3f);
        } else {
          Painter::buildArrow(linesRed, base, vec, 0.3f);
        }
      }
    }
  }

  // Draw normal vectors
  painter.setColor(Color::red());
  painter.drawLines(linesRed);
  painter.setColor(Color::yellow());
  painter.drawLines(linesYellow);
}

} // namespace wind