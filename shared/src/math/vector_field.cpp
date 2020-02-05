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
    : m_width(width), m_height(height), m_depth(depth), m_cellSize(cellsize) {
  using namespace bs;

  m_fieldSize = m_width * m_height * m_depth;
  m_field = new Vector3[m_fieldSize];
}

// -------------------------------------------------------------------------- //

VectorField::~VectorField() { delete m_field; }

// -------------------------------------------------------------------------- //

void VectorField::debugDraw() {
  bs::Vector<bs::Vector3> points;

  // Draw lines parallell with the x-axis
  const f32 xStart = 0;
  const f32 xEnd = m_cellSize * m_width;
  for (u32 z = 0; z < m_depth + 1; z++) {
    for (u32 y = 0; y < m_height + 1; y++) {
      points.push_back(bs::Vector3(xStart, f32(y), f32(z)));
      points.push_back(bs::Vector3(xEnd, f32(y), f32(z)));
    }
  }

  // Draw lines parallell with the x-axis
  const f32 yStart = 0;
  const f32 yEnd = m_cellSize * m_height;
  for (u32 z = 0; z < m_depth + 1; z++) {
    for (u32 x = 0; x < m_width + 1; x++) {
      points.push_back(bs::Vector3(f32(x), yStart, f32(z)));
      points.push_back(bs::Vector3(f32(x), yEnd, f32(z)));
    }
  }

  // Draw lines parallell with the x-axis
  const f32 zStart = 0;
  const f32 zEnd = m_cellSize * m_depth;
  for (u32 y = 0; y < m_height + 1; y++) {
    for (u32 x = 0; x < m_width + 1; x++) {
      points.push_back(bs::Vector3(f32(x), f32(y), zStart));
      points.push_back(bs::Vector3(f32(x), f32(y), zEnd));
    }
  }

  bs::DebugDraw::instance().drawLineList(points);
}

// -------------------------------------------------------------------------- //

bs::Vector3 &VectorField::GetVector(u32 x, u32 y, u32 z) {
  const u32 offset = x + (m_width * y) + (m_width * m_height * z);
  return m_field[offset];
}

// -------------------------------------------------------------------------- //

const bs::Vector3 &VectorField::GetVector(u32 x, u32 y, u32 z) const {
  const u32 offset = x + (m_width * y) + (m_width * m_height * z);
  return m_field[offset];
}

} // namespace wind
