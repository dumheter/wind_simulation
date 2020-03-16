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

#pragma once

// ========================================================================== //
// Header
// ========================================================================== //

#include "shared/math/math.hpp"
#include "shared/render/color.hpp"

// ========================================================================== //
// Painter Declaration
// ========================================================================== //

namespace wind {

/// Class that represents a painter for primarily debug objects on the screen.
class Painter {
  friend class App;

public:
  /// Arrow
  struct Arrow {
    Vec3F pos; ///< Position of arrow (center)
    Vec3F dir; ///< Direction of arrow. With magnitude taken into consideration.
  };

  // Line
  struct Line {
    Vec3F start; ///< Start of line
    Vec3F end;   ///< End of line
  };

  /// Construct painter
  Painter();

  /// Draw arrow at the specified position. The direction also determines the
  /// magnitude.
  void drawArrow(const Vec3F &pos, const Vec3F &dir, f32 scale = 1.0f);

  ///
  void drawLine(const Vec3F &start, const Vec3F &end);

  ///
  void drawLines(const bs::Vector<Vec3F> &lines);

  /// Draw an arrow
  void draw(const Arrow &arrow, f32 scale = 1.0f);

  /// Draw a line
  void draw(const Line &line);

  /// Build an arrow by adding the necessary lines to the list.
  static void buildArrow(bs::Vector<Vec3F> &lines, const Vec3F &pos,
                         const Vec3F &dir, f32 scale = 1.0f);

  /// Build an arrow by adding the necessary lines to the list.
  void build(bs::Vector<Vec3F> &lines, const Arrow &arrow, f32 scale = 1.0f);

  /// Set the color
  void setColor(Color color);

private:
  /// Begin drawing. This clears the object lists.
  void begin();

  /// End painting. This draws the objects to the screen.
  void end();

private:
  /// Lines to draw.
  bs::Vector<Vec3F> m_lines;
  /// Current brush color
  Color m_color = Color::white();
};

} // namespace wind