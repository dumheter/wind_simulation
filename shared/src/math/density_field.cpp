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

#include "math/density_field.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "math/math.hpp"

// ========================================================================== //
// VectorField Implementation
// ========================================================================== //

namespace wind {

DensityField::DensityField(u32 width, u32 height, u32 depth, f32 cellsize)
    : Field(width, height, depth, cellsize) {
  using namespace bs;
}

// -------------------------------------------------------------------------- //

void DensityField::debugDrawObject(const bs::Vector3 &offset) {
  bs::DebugDraw::instance().setColor(bs::Color::Blue);

  // Draw vectors
  for (u32 z = 0; z < m_dim.depth; z++) {
    const f32 zPos = offset.z + (z * m_cellSize);
    for (u32 y = 0; y < m_dim.height; y++) {
      const f32 yPos = offset.y + (y * m_cellSize);
      for (u32 x = 0; x < m_dim.width; x++) {
        const f32 xPos = offset.x + (x * m_cellSize);
        const bs::Vector3 base(xPos + (m_cellSize / 2.0f),
                               yPos + (m_cellSize / 2.0f),
                               zPos + (m_cellSize / 2.0f));
        f32 density = get(x, y, z);
        density = clamp(density, 0, 1.0f);
        bs::DebugDraw::instance().drawCube(
            base, bs::Vector3::ONE * (density * m_cellSize * 0.9f * 0.5f));
      }
    }
  }
}

// -------------------------------------------------------------------------- //

f32 DensityField::getSafe(s32 x, s32 y, s32 z) {
  bool bx = !isInBoundsX(x);
  bool by = !isInBoundsY(y);
  bool bz = !isInBoundsZ(z);

  s32 _x = clamp(x, 0, s32(m_dim.width) - 1);
  s32 _y = clamp(y, 0, s32(m_dim.height) - 1);
  s32 _z = clamp(z, 0, s32(m_dim.depth) - 1);

  if (bx && by && bz) { // X, Y and Z
    f32 comb = getSafe(_x, y, z) + getSafe(x, _y, z) + getSafe(x, y, _z);
    return 1.0f / 3.0f * comb;
  } else if (bx && by) { // X and Y
    f32 comb = getSafe(_x, y, z) + getSafe(x, _y, z);
    return 1.0f / 2.0f * comb;
  } else if (bx && bz) { // X and Z
    f32 comb = getSafe(_x, y, z) + getSafe(x, y, _z);
    return 1.0f / 2.0f * comb;
  } else if (by && bz) { // Y and Z
    f32 comb = getSafe(x, _y, z) + getSafe(x, y, _z);
    return 1.0f / 2.0f * comb;
  } else if (bx) { // X
    return get(_x, y, z);
  } else if (by) { // Y
    return get(x, _y, z);
  } else if (bz) { // Z
    return get(x, y, _z);
  } else { // None
    return get(x, y, z);
  }
}

} // namespace wind
