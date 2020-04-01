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

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/macros.hpp"
#include "shared/math/field.hpp"
#include "shared/sim/obstruction_field.hpp"
#include "shared/types.hpp"

// ========================================================================== //
// VectorField Declaration
// ========================================================================== //

namespace wind {

WIND_FORWARD_DECLARE(Painter);

// -------------------------------------------------------------------------- //

/// Class that represents a vector field
class VectorField : public FieldBase {
public:
  /// Type of the three vector field components (Each component is its own
  /// field)
  struct Comp : Field<f32> {
    /// Construct vector-field component field
    Comp(u32 width, u32 height, u32 depth, f32 cellSize)
        : Field(width, height, depth, cellSize) {}

    /// Not used as components are not painted separately
    void paintT(Painter &painter, const Vec3F &offset,
                const Vec3F &padding) const override {}
  };

public:
  /* Construct a vector-field with the specified 'width', 'height' and
   * 'depth' (in number of cells). The size of a cell (in meters) can also be
   * specified.  */
  VectorField(u32 width, u32 height, u32 depth, f32 cellSize = 1.0f);

  /// \copydoc FieldBase::paint
  void paint(Painter &painter, const Vec3F &offset,
             const Vec3F &padding) const override {}

  /// Paint the vector field by drawing an arrow for each vector
  void paintWithObstr(Painter &painter, const ObstructionField &obstrField,
                      const Vec3F &offset = Vec3F(),
                      const Vec3F &padding = Vec3F(0, 0, 0)) const;

  /// Returns the X component of the vector field
  Comp *getX() { return m_x; }

  /// Returns the X component of the vector field
  const Comp *getX() const { return m_x; }

  /// Returns the Y component of the vector field
  Comp *getY() { return m_y; }

  /// Returns the Y component of the vector field
  const Comp *getY() const { return m_y; }

  /// Returns the Z component of the vector field
  Comp *getZ() { return m_z; }

  /// Returns the Z component of the vector field
  const Comp *getZ() const { return m_z; }

  /// Returns a vector for the specified offset in the vector field. This is
  /// built on demand from the separate vector components.
  Vec3F get(u32 offset) const {
    return Vec3F(getX()->get(offset), getY()->get(offset), getZ()->get(offset));
  }

  /// Returns a vector for the specified location in the vector field. This is
  /// built on demand from the separate vector components.
  /// \pre 'x', 'y' and 'z' must be '> 1' and '< dim-1'.
  Vec3F get(s32 x, s32 y, s32 z) const {
    return Vec3F(getX()->get(x, y, z), getY()->get(x, y, z),
                 getZ()->get(x, y, z));
  }

  /// Returns a vector for the specified location in the vector field. This is
  /// built on demand from the separate vector components.
  /// \pre 'x', 'y' and 'z' must be '> 1' and '< dim-1'.
  Vec3F get(const Pos &pos) const {
    return Vec3F(getX()->get(pos.x, pos.y, pos.z),
                 getY()->get(pos.x, pos.y, pos.z),
                 getZ()->get(pos.x, pos.y, pos.z));
  }

  /// Returns the vector for a given position specified in meters.
  Vec3F getM(s32 x, s32 y, s32 z) const;

  /// Returns a vector for the given location. The location should be specified
  /// in meters.
  // Vec3F getInMeter(s32 x, s32 y, s32 z);

  /// \param point Position in local meters. Must be > 1 and < dim-1.
  /// \note If point is out of bounds, Vec3F::ZERO is returned.
  Vec3F sampleNear(Vec3F point) const;

  /// Set the vector at the specified location in the vector field.
  void set(u32 offset, Vec3F vec) {
    m_x->get(offset) = vec.x;
    m_y->get(offset) = vec.y;
    m_z->get(offset) = vec.z;
  }

  /// Set the vector at the specified location in the vector field.
  void set(s32 x, s32 y, s32 z, Vec3F vec) {
    m_x->get(x, y, z) = vec.x;
    m_y->get(x, y, z) = vec.y;
    m_z->get(x, y, z) = vec.z;
  }

private:
  /// Vector X component
  Comp *m_x;
  /// Vector Y component
  Comp *m_y;
  /// Vector Z component
  Comp *m_z;
};

} // namespace wind
