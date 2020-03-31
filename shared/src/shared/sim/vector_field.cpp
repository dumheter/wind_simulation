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

#include "shared/sim/vector_field.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/math/math.hpp"
#include "shared/render/painter.hpp"

#include <dlog/dlog.hpp>

// ========================================================================== //
// VectorField Implementation
// ========================================================================== //

namespace wind {

VectorField::VectorField(u32 width, u32 height, u32 depth, f32 cellSize)
    : FieldBase(width, height, depth, cellSize) {
  using namespace bs;

  m_x = new Comp(width, height, depth, cellSize);
  m_y = new Comp(width, height, depth, cellSize);
  m_z = new Comp(width, height, depth, cellSize);
  m_dim = m_x->getDim();
  m_cellSize = m_x->getCellSize();
}

// -------------------------------------------------------------------------- //

void VectorField::paintWithObstr(Painter &painter,
                                 const ObstructionField *obstrField,
                                 const Vec3F &offset,
                                 const Vec3F &padding) const {
  bs::Vector<bs::Vector3> linesRed;
  // bs::Vector<bs::Vector3> linesYellow;

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
        if (obstrField->get(x, y, z)) {
          // Painter::buildArrow(linesYellow, base, vec, 0.3f);
        } else {
          Painter::buildArrow(linesRed, base, vec, 0.3f);
        }
      }
    }
  }

  // Draw normal vectors
  painter.setColor(Color::red());
  painter.drawLines(linesRed);
  // painter.setColor(Color::yellow());
  // painter.drawLines(linesYellow);
}

struct Cell {
  s32 x, y, z;

  Cell() = default;
  Cell(f32 cellSize, Vec3F point)
      : x(point.x * cellSize), y(point.y * cellSize), z(point.z * cellSize) {}

  Vec3F toVec3F() const {
    return Vec3F{static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(z)};
  }
};

Vec3F VectorField::sampleNear(Vec3F point) const {
  Cell cell[8];
  cell[0] = Cell{m_cellSize, point};
  cell[1] = Cell{m_cellSize, point + Vec3F{0.0f, 0.0f, m_cellSize}};
  cell[2] = Cell{m_cellSize, point + Vec3F{0.0f, m_cellSize, 0.0f}};
  cell[3] = Cell{m_cellSize, point + Vec3F{0.0f, m_cellSize, m_cellSize}};
  cell[4] = Cell{m_cellSize, point + Vec3F{m_cellSize, 0.0f, 0.0f}};
  cell[5] = Cell{m_cellSize, point + Vec3F{m_cellSize, 0.0f, m_cellSize}};
  cell[6] = Cell{m_cellSize, point + Vec3F{m_cellSize, m_cellSize, 0.0f}};
  cell[7] = Cell{m_cellSize, point + Vec3F{m_cellSize, m_cellSize, m_cellSize}};

  f32 dist[8];
  f32 sum = 0.0f;
  for (u32 i = 0; i < 8; ++i) {
    dist[i] = distance(point, cell[i].toVec3F());
    // using gaussian makes sure the closest point is valued the most.
    sum += gaussian(dist[i], 1.0f, 0.0f, m_cellSize / 2.0f);
  }

  Vec3F force = Vec3F::ZERO;
  for (u32 i = 0; i < 8; ++i) {
    if (cell[i].x >= static_cast<s32>(m_dim.width) ||
        cell[i].y >= static_cast<s32>(m_dim.height) ||
        cell[i].z >= static_cast<s32>(m_dim.depth) || cell[i].x < 0 ||
        cell[i].y < 0 || cell[i].z < 0) {
      continue;
    }
    const f32 fraction = gaussian(dist[i], 1.0f, 0.0f, m_cellSize / 2.0f) / sum;
    force += fraction * get(cell[i].x, cell[i].y, cell[i].z);
  }

  return force;
}

} // namespace wind
