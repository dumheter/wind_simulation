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

#include "shared/math/spline.hpp"

#include <Debug/BsDebugDraw.h>

// ========================================================================== //
// Painter Implementation
// ========================================================================== //

namespace wind {

Painter::Painter() {}

// -------------------------------------------------------------------------- //

void Painter::drawCube(const Vec3F &pos, const Vec3F &dim) {
  buildCube(m_lines, pos, dim);
}

// -------------------------------------------------------------------------- //

void Painter::drawCross(const Vec3F &pos, const Vec3F &dim) {
  buildCross(m_lines, pos, dim);
}

// -------------------------------------------------------------------------- //

void Painter::drawGizmo(const Vec3F &pos, f32 scale) {
  const Color col = m_color;
  setColor(Color::red());
  drawArrow(pos, Vec3F(1, 0, 0), scale * 2.0f);
  setColor(Color::green());
  drawArrow(pos, Vec3F(0, 1, 0), scale * 2.0f);
  setColor(Color::blue());
  drawArrow(pos, Vec3F(0, 0, 1), scale * 2.0f);
  setColor(col);
}

// -------------------------------------------------------------------------- //

void Painter::drawArrow(const Vec3F &pos, const Vec3F &dir, f32 scale) {
  buildArrow(m_lines, pos, dir, scale);
}

// -------------------------------------------------------------------------- //

void Painter::drawLine(const Vec3F &start, const Vec3F &end) {
  m_lines.push_back(start);
  m_lines.push_back(end);
}

void Painter::drawSpline(const Spline &spline, u32 samples) {
  buildSpline(m_lines, spline, samples);
}

// -------------------------------------------------------------------------- //

void Painter::drawLines(const bs::Vector<Vec3F> &lines) {
  bs::DebugDraw::instance().drawLineList(lines);
}

// -------------------------------------------------------------------------- //

void Painter::draw(const Cube &cube) { drawCube(cube.pos, cube.dim); }

// -------------------------------------------------------------------------- //

void Painter::draw(const Cross &cross) { drawCross(cross.pos, cross.dim); }

// -------------------------------------------------------------------------- //

void Painter::draw(const Arrow &arrow, f32 scale) {
  drawArrow(arrow.pos, arrow.dir, scale);
}

// -------------------------------------------------------------------------- //

void Painter::draw(const Line &line) { drawLine(line.start, line.end); }

// -------------------------------------------------------------------------- //

void Painter::buildCube(bs::Vector<Vec3F> &lines, const Vec3F &pos,
                        const Vec3F &dim) {
  const f32 xmin = pos.x - dim.x / 2.0f;
  const f32 xmax = pos.x + dim.x / 2.0f;
  const f32 ymin = pos.y - dim.y / 2.0f;
  const f32 ymax = pos.y + dim.y / 2.0f;
  const f32 zmin = pos.z - dim.z / 2.0f;
  const f32 zmax = pos.z + dim.z / 2.0f;

  // Bottom points
  const Vec3F p0(xmin, ymin, zmin);
  const Vec3F p1(xmin, ymin, zmax);
  const Vec3F p2(xmax, ymin, zmax);
  const Vec3F p3(xmax, ymin, zmin);
  lines.push_back(p0);
  lines.push_back(p1);
  lines.push_back(p1);
  lines.push_back(p2);
  lines.push_back(p2);
  lines.push_back(p3);
  lines.push_back(p3);
  lines.push_back(p0);

  // Top points
  const Vec3F p4(xmin, ymax, zmin);
  const Vec3F p5(xmin, ymax, zmax);
  const Vec3F p6(xmax, ymax, zmax);
  const Vec3F p7(xmax, ymax, zmin);
  lines.push_back(p4);
  lines.push_back(p5);
  lines.push_back(p5);
  lines.push_back(p6);
  lines.push_back(p6);
  lines.push_back(p7);
  lines.push_back(p7);
  lines.push_back(p4);

  // Joins
  lines.push_back(p0);
  lines.push_back(p4);

  lines.push_back(p1);
  lines.push_back(p5);

  lines.push_back(p2);
  lines.push_back(p6);

  lines.push_back(p3);
  lines.push_back(p7);
}

// -------------------------------------------------------------------------- //

void Painter::buildCross(bs::Vector<Vec3F> &lines, const Vec3F &pos,
                         const Vec3F &dim) {
  const Vec3F x0 = pos - Vec3F(dim.x / 2.0f, 0, 0);
  const Vec3F x1 = pos + Vec3F(dim.x / 2.0f, 0, 0);
  const Vec3F y0 = pos - Vec3F(0, dim.y / 2.0f, 0);
  const Vec3F y1 = pos + Vec3F(0, dim.y / 2.0f, 0);
  const Vec3F z0 = pos - Vec3F(0, 0, dim.z / 2.0f);
  const Vec3F z1 = pos + Vec3F(0, 0, dim.z / 2.0f);

  lines.push_back(x0);
  lines.push_back(x1);
  lines.push_back(y0);
  lines.push_back(y1);
  lines.push_back(z0);
  lines.push_back(z1);
}

// -------------------------------------------------------------------------- //

void Painter::buildArrow(bs::Vector<Vec3F> &lines, const Vec3F &pos,
                         const Vec3F &dir, f32 scale) {
  Vec3F dirNorm = dir;
  dirNorm.normalize();

  // Main arrow line
  const Vec3F aStart = pos - scale * (dir / 4.0f);
  const Vec3F aEnd = pos + scale * (dir / 4.0f);
  lines.push_back(aStart);
  lines.push_back(aEnd);

  // Vectors perpendicular to direction
  Vec3F perp0 = dir.cross(Vec3F(0.0f, 1.0f, 0.001f));
  perp0.normalize();
  perp0 *= 0.02f * scale;
  Vec3F perp1 = perp0.cross(dir);
  perp1.normalize();
  perp1 *= 0.02f * scale;

  // Add corners
  const Vec3F aHeadStart = aEnd - dirNorm * 0.1f * scale;
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

void Painter::buildSpline(bs::Vector<Vec3F> &lines, const Spline &spline,
                          u32 samples) {
  const f32 len = spline.calcLen(u32(spline.getPoints().size()) * 10u);
  if (samples == kSplineSamplesAuto) {
    samples = u32(len);
  }

  Vec3F prev = spline.sample(0);
  const f32 step = 1.0f / samples;
  for (u32 i = 1; i <= samples; i++) {
    const f32 t = step * i;
    const Vec3F pos = spline.sample(t);
    lines.push_back(prev);
    lines.push_back(pos);
    prev = pos;
  }
}

// -------------------------------------------------------------------------- //

void Painter::build(bs::Vector<Vec3F> &lines, const Arrow &arrow, f32 scale) {
  buildArrow(lines, arrow.pos, arrow.dir, scale);
}

// -------------------------------------------------------------------------- //

void Painter::setColor(Color color) {
  // Flush
  if (m_color != color) {
    bs::DebugDraw::instance().drawLineList(m_lines);
    m_lines.clear();
    m_color = color;
  }

  // Set color
  const bs::Color _color(color.getRedF32(), color.getGreenF32(),
                         color.getBlueF32(), color.getAlphaF32());
  bs::DebugDraw::instance().setColor(_color);
}

// -------------------------------------------------------------------------- //

void Painter::begin() {
  m_lines.clear();
  bs::DebugDraw::instance().clear();
}

// -------------------------------------------------------------------------- //

void Painter::end() { bs::DebugDraw::instance().drawLineList(m_lines); }

} // namespace wind
