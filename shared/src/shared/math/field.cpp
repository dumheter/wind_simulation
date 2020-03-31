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

#include "field.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/render/painter.hpp"

// ========================================================================== //
// Field Implementation
// ========================================================================== //

namespace wind {

FieldBase::FieldBase(u32 width, u32 height, u32 depth, f32 cellSize)
    : m_dim({width, height, depth}), m_cellSize(cellSize),
      m_cellCount(width * height * depth) {}

// -------------------------------------------------------------------------- //

void FieldBase::paintFrame(Painter &painter, const Vec3F &offset,
                           const Vec3F &padding) const {
  const u32 xPad = 2 * u32(padding.x);
  const u32 yPad = 2 * u32(padding.y);
  const u32 zPad = 2 * u32(padding.z);

  bs::Vector<bs::Vector3> points;

  // Draw lines parallel with the x-axis
  const f32 xStart = offset.x;
  const f32 xEnd = xStart + (m_cellSize * (m_dim.width - xPad));
  for (u32 z = 0; z < m_dim.depth + 1 - zPad; z++) {
    const f32 zPos = offset.z + (z * m_cellSize);
    for (u32 y = 0; y < m_dim.height + 1 - yPad; y++) {
      const f32 yPos = offset.y + (y * m_cellSize);
      points.emplace_back(bs::Vector3(xStart, yPos, zPos));
      points.emplace_back(bs::Vector3(xEnd, yPos, zPos));
    }
  }

  // Draw lines parallel with the y-axis
  const f32 yStart = offset.y;
  const f32 yEnd = yStart + (m_cellSize * (m_dim.height - yPad));
  for (u32 z = 0; z < m_dim.depth + 1 - zPad; z++) {
    const f32 zPos = offset.z + (z * m_cellSize);
    for (u32 x = 0; x < m_dim.width + 1 - xPad; x++) {
      const f32 xPos = offset.x + (x * m_cellSize);
      points.emplace_back(bs::Vector3(xPos, yStart, zPos));
      points.emplace_back(bs::Vector3(xPos, yEnd, zPos));
    }
  }

  // Draw lines parallel with the z-axis
  const f32 zStart = offset.z;
  const f32 zEnd = zStart + (m_cellSize * (m_dim.depth - zPad));
  for (u32 y = 0; y < m_dim.height + 1 - yPad; y++) {
    const f32 yPos = offset.y + (y * m_cellSize);
    for (u32 x = 0; x < m_dim.width + 1 - xPad; x++) {
      const f32 xPos = offset.x + (x * m_cellSize);
      points.emplace_back(bs::Vector3(xPos, yPos, zStart));
      points.emplace_back(bs::Vector3(xPos, yPos, zEnd));
    }
  }

  painter.setColor(Color::white());
  painter.drawLines(points);
}

} // namespace wind
