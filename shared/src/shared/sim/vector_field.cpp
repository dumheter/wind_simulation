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

void VectorField::paint(Painter &painter, const Vec3F &offset,
                        const Vec3F &padding) const {
  bs::Vector<bs::Vector3> lines;

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
        Painter::buildArrow(lines, base, vec * 0.5f, 0.5f);
      }
    }
  }

  // Draw normal vectors
  painter.setColor(Color::green());
  painter.drawLines(lines);
}

// -------------------------------------------------------------------------- //

void VectorField::paintWithObstr(Painter &painter,
                                 const ObstructionField &obstrField,
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
        if (obstrField.get(x, y, z)) {
          // Painter::buildArrow(linesYellow, base, vec, 0.3f);
        } else {
          Painter::buildArrow(linesRed, base, vec * 0.5f, 0.5f);
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

// -------------------------------------------------------------------------- //

Vec3F VectorField::getM(s32 x, s32 y, s32 z) const {
  return get(s32(x / m_cellSize), s32(y / m_cellSize), s32(z / m_cellSize));
}

// -------------------------------------------------------------------------- //

Vec3F VectorField::sampleNear(Vec3F point) const {
  static constexpr u32 kCellCount = 8;

  const f32 cellX = point.x / m_cellSize;
  const f32 cellY = point.y / m_cellSize;
  const f32 cellZ = point.z / m_cellSize;

  // Setup 8-closest cell positions
  Pos cells[kCellCount];
  cells[0] = Pos{s32(cellX), s32(cellY), s32(cellZ)};
  cells[1] = Pos{s32(cellX), s32(cellY), s32(cellZ) + 1};
  cells[2] = Pos{s32(cellX), s32(cellY) + 1, s32(cellZ)};
  cells[3] = Pos{s32(cellX), s32(cellY) + 1, s32(cellZ) + 1};
  cells[4] = Pos{s32(cellX) + 1, s32(cellY), s32(cellZ)};
  cells[5] = Pos{s32(cellX) + 1, s32(cellY), s32(cellZ) + 1};
  cells[6] = Pos{s32(cellX) + 1, s32(cellY) + 1, s32(cellZ)};
  cells[7] = Pos{s32(cellX) + 1, s32(cellY) + 1, s32(cellZ) + 1};

  // Distances in meter to weight by
  f32 dists[kCellCount];
  f32 sum = 0.0f;

  // Calculate ditance from cells to sample point
  for (u32 i = 0; i < kCellCount; i++) {
    dists[i] =
        distance(Vec3F{cellX, cellY, cellZ},
                 Vec3F{f32(cells[i].x), f32(cells[i].y), f32(cells[i].z)}) *
        m_cellSize;
    sum += gaussian(dists[i], 1.0f, 0.0f, m_cellSize / 2.0f);
  }

  // Calculate force
  Vec3F force{0.0f, 0.0f, 0.0f};
  for (u32 i = 0; i < kCellCount; ++i) {
    if (!inBounds(cells[i])) {
      continue;
    }
    const f32 fraction =
        gaussian(dists[i], 1.0f, 0.0f, m_cellSize / 2.0f) / sum;
    force += fraction * get(cells[i]);
  }

  return force;
}

} // namespace wind
