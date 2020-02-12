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

#include "sim/density_field.hpp"

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

void DensityField::debugDrawObject(const Vec3F &offset, const Vec3F &padding) {
  bs::DebugDraw::instance().setColor(bs::Color::Blue);

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
        f32 density = get(x, y, z);
        density = clamp(density, 0, 1.0f);

#if 1
        bs::DebugDraw::instance().drawCube(
            base, bs::Vector3::ONE * (density * m_cellSize * 0.9f * 0.5f));
#else
        f32 d000 = clamp(getClamped(x, y, z), 0.0f, 1.0f);
        f32 d010 = clamp(getClamped(x, y + 1, z), 0.0f, 1.0f);
        f32 d100 = clamp(getClamped(x + 1, y, z), 0.0f, 1.0f);
        f32 d110 = clamp(getClamped(x + 1, y + 1, z), 0.0f, 1.0f);
        f32 d001 = clamp(getClamped(x, y, z + 1), 0.0f, 1.0f);
        f32 d011 = clamp(getClamped(x, y + 1, z + 1), 0.0f, 1.0f);
        f32 d101 = clamp(getClamped(x + 1, y, z + 1), 0.0f, 1.0f);
        f32 d111 = clamp(getClamped(x + 1, y + 1, z + 1), 0.0f, 1.0f);

        f32 posOffset = 0.3f;
        f32 negOffset = -0.3f;

        // D000
        bs::DebugDraw::instance().setColor(bs::Color(d000, d000, d000));
        bs::DebugDraw::instance().drawCube(
            bs::Vector3(xPos + negOffset, yPos + negOffset, zPos + negOffset),
            bs::Vector3::ONE * 0.2f);

        // D010
        bs::DebugDraw::instance().setColor(bs::Color(d010, d010, d010));
        bs::DebugDraw::instance().drawCube(
            bs::Vector3(xPos + negOffset, yPos + posOffset, zPos + negOffset),
            bs::Vector3::ONE * 0.2f);

        // D100
        bs::DebugDraw::instance().setColor(bs::Color(d100, d100, d100));
        bs::DebugDraw::instance().drawCube(
            bs::Vector3(xPos + posOffset, yPos + negOffset, zPos + negOffset),
            bs::Vector3::ONE * 0.2f);

        // D110
        bs::DebugDraw::instance().setColor(bs::Color(d110, d110, d110));
        bs::DebugDraw::instance().drawCube(
            bs::Vector3(xPos + posOffset, yPos + posOffset, zPos + negOffset),
            bs::Vector3::ONE * 0.2f);

        // D001
        bs::DebugDraw::instance().setColor(bs::Color(d001, d001, d001));
        bs::DebugDraw::instance().drawCube(
            bs::Vector3(xPos + negOffset, yPos + negOffset, zPos + posOffset),
            bs::Vector3::ONE * 0.2f);

        // D011
        bs::DebugDraw::instance().setColor(bs::Color(d011, d011, d011));
        bs::DebugDraw::instance().drawCube(
            bs::Vector3(xPos + negOffset, yPos + posOffset, zPos + posOffset),
            bs::Vector3::ONE * 0.2f);

        // D101
        bs::DebugDraw::instance().setColor(bs::Color(d101, d101, d101));
        bs::DebugDraw::instance().drawCube(
            bs::Vector3(xPos + posOffset, yPos + negOffset, zPos + posOffset),
            bs::Vector3::ONE * 0.2f);

        // D111
        bs::DebugDraw::instance().setColor(bs::Color(d111, d111, d111));
        bs::DebugDraw::instance().drawCube(
            bs::Vector3(xPos + posOffset, yPos + posOffset, zPos + posOffset),
            bs::Vector3::ONE * 0.2f);
#endif
      }
    }
  }
}

} // namespace wind
