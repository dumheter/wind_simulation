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

#include "vector_field.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "math/math.hpp"

#include <Debug/BsDebugDraw.h>

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

void VectorField::debugDraw(const Vec3F &offset,
                            ObstructionField *obstructionField, bool drawFrame,
                            const Vec3F &padding) {
  bs::Vector<bs::Vector3> points;
  bs::Vector<bs::Vector3> pointsHead;

  bs::Vector<bs::Vector3> points0;
  bs::Vector<bs::Vector3> pointsHead0;

  u32 xPad = u32(padding.x);
  u32 yPad = u32(padding.y);
  u32 zPad = u32(padding.z);

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
        const bs::Vector3 first = base + (vec * .2f);
        const bs::Vector3 second = first + (vec * .05f);

        if (obstructionField->get(x, y, z)) {
          points0.push_back(base);
          points0.push_back(first);
          pointsHead0.push_back(first);
          pointsHead0.push_back(second);
        } else {
          points.push_back(base);
          points.push_back(first);
          pointsHead.push_back(first);
          pointsHead.push_back(second);
        }
      }
    }
  }

  // Draw normal vectors
  bs::DebugDraw::instance().setColor(bs::Color::Red);
  bs::DebugDraw::instance().drawLineList(points);
  bs::DebugDraw::instance().setColor(bs::Color::Blue);
  bs::DebugDraw::instance().drawLineList(pointsHead);

  // Draw obstructed vectors
  bs::DebugDraw::instance().setColor(bs::Color(1.0f, 1.0f, 0.0f));
  bs::DebugDraw::instance().drawLineList(points0);
  bs::DebugDraw::instance().setColor(bs::Color::Green);
  bs::DebugDraw::instance().drawLineList(pointsHead0);

  // Draw frame
  if (drawFrame) {
    m_x->debugDrawFrame();
  }
}

} // namespace wind
