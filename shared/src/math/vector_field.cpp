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

#include <Debug/BsDebugDraw.h>

// ========================================================================== //
// VectorField Implementation
// ========================================================================== //

namespace wind {

VectorField::VectorField(u32 width, u32 height, u32 depth, f32 cellsize)
    : Field(width, height, depth, cellsize) {
  using namespace bs;

  for (u32 i = 0; i < m_dataSize; i++) {
    m_data[i] = bs::Vector3(0, 1, 1);
  }
}

// -------------------------------------------------------------------------- //

void VectorField::debugDrawObject(const bs::Vector3 &offset) {
  bs::Vector<bs::Vector3> points;

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
        const bs::Vector3 &vec = get(x, y, z);

        const bs::Vector3 start = base - (vec * .1f);
        const bs::Vector3 end = base + (vec * .1f);
        points.push_back(start);
        points.push_back(end);
      }
    }
  }

  bs::DebugDraw::instance().setColor(bs::Color::Red);
  bs::DebugDraw::instance().drawLineList(points);
}

// -------------------------------------------------------------------------- //

bs::Vector3 VectorField::getSafe(s32 x, s32 y, s32 z) {
  switch (m_borderKind) {
    // Default border value
  case BorderKind::DEFAULT: {
    return isInBounds(x, y, z) ? get(x, y, z) : m_borderDefaultValue;
  }
    // Contained border vectors
  case BorderKind::CONTAINED: {
    bool rx = !isInBoundsX(x);
    bool ry = !isInBoundsY(y);
    bool rz = !isInBoundsZ(z);
    x = Clamp(x, 0, s32(m_dim.width) - 1);
    y = Clamp(y, 0, s32(m_dim.height) - 1);
    z = Clamp(z, 0, s32(m_dim.depth) - 1);
    bs::Vector3 &v = get(x, y, z);
    if (rx) {
      v.x = 0;
    }
    if (ry) {
      v.y = 0;
    }
    if (rz) {
      v.z = 0;
    }
    return v;
  }
    // Empty border value
  case BorderKind::BLOCKED: {
    return isInBounds(x, y, z) ? get(x, y, z) : bs::Vector3();
  }
  }

  // CANNOT HAPPEN!
  exit(-234);
  return bs::Vector3::ZERO;
}

// -------------------------------------------------------------------------- //

void VectorField::setBorder(BorderKind kind) { m_borderKind = kind; }

// -------------------------------------------------------------------------- //

void VectorField::setBorderDefaultValue(const bs::Vector3 &value) {
  m_borderDefaultValue = value;
}

} // namespace wind
