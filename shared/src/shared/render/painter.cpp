// MIT License
//
// Copyright (c) 2019-2020 Filip Björklund, Christoffer Gustafsson
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

#include "painter.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <Debug/BsDebugDraw.h>

// ========================================================================== //
// Painter Implementation
// ========================================================================== //

namespace wind {

Painter::Painter() {}

// -------------------------------------------------------------------------- //

void Painter::drawArrow(const Vec3F &pos, const Vec3F &dir) {
  buildArrow(m_lines, pos, dir);
}

// -------------------------------------------------------------------------- //

void Painter::drawLine(const Vec3F &start, const Vec3F &end) {
  m_lines.push_back(start);
  m_lines.push_back(end);
}

// -------------------------------------------------------------------------- //

void Painter::drawLines(const bs::Vector<Vec3F> &lines) {
  bs::DebugDraw::instance().drawLineList(lines);
}

// -------------------------------------------------------------------------- //

void Painter::draw(const Arrow &arrow) { drawArrow(arrow.pos, arrow.dir); }

// -------------------------------------------------------------------------- //

void Painter::draw(const Line &line) { drawLine(line.start, line.end); }

// -------------------------------------------------------------------------- //

void Painter::buildArrow(bs::Vector<Vec3F> &lines, const Vec3F &pos,
                         const Vec3F &dir) {
  Vec3F dirNorm = dir;
  dirNorm.normalize();

  // Main arrow line
  const Vec3F aStart = pos - dir / 4.0f;
  const Vec3F aEnd = pos + dir / 4.0f;
  lines.push_back(aStart);
  lines.push_back(aEnd);

  // Vectors perpendicular to direction
  Vec3F perp0 = dir.cross(Vec3F(0.0f, 1.0f, 0.0f));
  perp0.normalize();
  perp0 *= 0.05f;
  Vec3F perp1 = perp0.cross(dir);
  perp1.normalize();
  perp1 *= 0.05f;

  // Add corners
  const Vec3F aHeadStart = aEnd - dirNorm * 0.2f;
  const Vec3F corner0 = aHeadStart + perp0 + perp1;
  const Vec3F corner1 = aHeadStart - perp0 + perp1;
  const Vec3F corner2 = aHeadStart + perp0 - perp1;
  const Vec3F corner3 = aHeadStart - perp0 - perp1;
  lines.push_back(corner0);
  lines.push_back(aEnd);
  lines.push_back(corner1);
  lines.push_back(aEnd);
  lines.push_back(corner2);
  lines.push_back(aEnd);
  lines.push_back(corner3);
  lines.push_back(aEnd);

  // Connect corners
  lines.push_back(corner0);
  lines.push_back(corner1);
  lines.push_back(corner1);
  lines.push_back(corner3);
  lines.push_back(corner3);
  lines.push_back(corner2);
  lines.push_back(corner2);
  lines.push_back(corner0);
}

// -------------------------------------------------------------------------- //

void Painter::build(bs::Vector<Vec3F> &lines, const Arrow &arrow) {
  buildArrow(lines, arrow.pos, arrow.dir);
}

// -------------------------------------------------------------------------- //

void Painter::setColor(Color color) {
  // Flush
  if (m_color != color) {
    end();
    begin();
    m_color = color;
  }

  // Set color
  const bs::Color _color(color.getRedF32(), color.getGreenF32(),
                         color.getBlueF32(), color.getAlphaF32());
  bs::DebugDraw::instance().setColor(_color);
}

// -------------------------------------------------------------------------- //

void Painter::begin() { m_lines.clear(); }

// -------------------------------------------------------------------------- //

void Painter::end() { bs::DebugDraw::instance().drawLineList(m_lines); }

} // namespace wind
