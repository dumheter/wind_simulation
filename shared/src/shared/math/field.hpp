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
#include "shared/math/math.hpp"
#include "shared/types.hpp"

// ========================================================================== //
// FieldBase Declaration
// ========================================================================== //

namespace wind {

WIND_FORWARD_DECLARE(Painter);

// -------------------------------------------------------------------------- //

/// Class that represents a base for a mathematical field in three dimensions
/// with a 'width', 'height' and 'depth'. The total memory consumption of a
/// field depends on the size of a cell and the type parameter of the 'Field'
/// class. Cell size determines the resolution of the field cells. By default
/// the size 1.0f is used which represents 1m^3.
///
/// The 'Field' class implements all functionality related to storing a type 'T'
/// in the field, while this class contains the base functionality such as debug
/// drawing.
class FieldBase {
public:
  /// Structure that represents a position in a field
  struct Pos {
    s32 x, y, z;
  };

  /// Structure that represents dimensions of a field
  struct Dim {
    u32 width;  ///< Width of dimension
    u32 height; ///< Height of dimension
    u32 depth;  ///< Depth of dimension

    /// Equality operator
    friend bool operator==(const Dim &d0, const Dim &d1) {
      return d0.width == d1.width && d0.height == d1.height &&
             d0.depth == d1.depth;
    }

    /// Inequality operator
    friend bool operator!=(const Dim &d0, const Dim &d1) { return !(d0 == d1); }
  };

  /// Construct field with dimensions and cell-size specified
  FieldBase(u32 width, u32 height, u32 depth, f32 cellSize = 1.0f);

  /// Destruct field
  virtual ~FieldBase() = default;

  /// Draw a debug representation of the field using lines.
  /// \brief Draw debug representation.
  /// \param painter Painter to draw with.
  /// \param offset Offset to draw the field at. Padding is subtracted from the
  /// offset.
  /// \param padding Padding on each side. Setting to (1,1,1) means that each
  /// side is padded with one cell each, giving a total of two extra cells on
  /// each axis
  virtual void paint(Painter &painter, const Vec3F &offset = Vec3F(0, 0, 0),
                     const Vec3F &padding = Vec3F(0, 0, 0)) const = 0;

  /// Paint a frame for the field. This can be used to visalize the cells in the
  /// field.
  void paintFrame(Painter &painter, const Vec3F &offset = Vec3F(0, 0, 0),
                  const Vec3F &padding = Vec3F(0, 0, 0)) const;

  /// Convert position into an index in the data of the field
  u32 fromPos(s32 x, s32 y, s32 z) const {
    assert(inBounds(x, y, z) && "Position cannot lie outside of field");
    return x + (m_dim.width * y) + (m_dim.width * m_dim.height * z);
  }

  /// Convert position into an index in the data of the field
  u32 fromPos(const Pos &pos) const { return fromPos(pos.x, pos.y, pos.z); }

  /// Convert from an index in the data to a position.
  /* Convert offset in data to position */
  Pos fromOffset(u32 offset) const {
    assert(offset < m_cellCount && "Offset cannot lie outside of field data");
    const s32 x = offset % m_dim.width;
    const s32 y = (offset % (m_dim.width * m_dim.height)) / m_dim.width;
    const s32 z = offset / (m_dim.width * m_dim.height);
    return Pos{x, y, z};
  }

  /// Returns whether or not a given position is on the edge of the field.
  bool onEdge(s32 x, s32 y, s32 z) const {
    assert(inBounds(x, y, z) && "Position cannot lie outside of field");
    return x == 0 || y == 0 || z == 0 || x == m_dim.width - 1 ||
           y == m_dim.height - 1 || z == m_dim.depth - 1;
  }

  /// Returns whether or not a given position is on the edge of the field.
  bool onEdge(const Pos &pos) const { return onEdge(pos.x, pos.y, pos.z); }

  /// Returns whether or not a position is inside the bounds of the field.
  bool inBounds(s32 x, s32 y, s32 z) const {
    return inBoundsX(x) && inBoundsY(y) && inBoundsZ(z);
  }

  /// Returns whether or not a position is inside the bounds of the field.
  bool inBounds(const Pos &pos) const { return inBounds(pos.x, pos.y, pos.z); }

  /// Returns whether or not a position is inside the 'x' bounds of the field.
  bool inBoundsX(s32 x) const { return x >= 0 && x < s32(m_dim.width); }

  /// Returns whether or not a position is inside the 'x' bounds of the field.
  bool inBoundsY(s32 y) const { return y >= 0 && y < s32(m_dim.height); }

  /// Returns whether or not a position is inside the 'z' bounds of the field.
  bool inBoundsZ(s32 z) const { return z >= 0 && z < s32(m_dim.depth); }

  /// Retrieve the size of cells in the field.
  f32 getCellSize() const { return m_cellSize; }

  /// Retrieve the dimension of the field.
  const Dim &getDim() const { return m_dim; }

  /// Retrieve the number of cells in the field.
  u32 getCellCount() const { return m_cellCount; }

protected:
  /// Dimensions of the field
  Dim m_dim;
  /// Cell size in meters
  f32 m_cellSize;
  /// Number of cells in field
  u32 m_cellCount;
};

} // namespace wind

// ========================================================================== //
// Field Declaration
// ========================================================================== //

namespace wind {

/* Class that represents a field 3-dimensional field of objects of the specified
 * type 'T'. The field consists of 'width' by 'height' by 'depth' number of
 * cells, where each cell stores a value of type 'T'.
 *
 * Sampling the field can be done in two ways. Either using the 'get' function
 * that simply returns the value in a cell given the 'x', 'y' and 'z' values. Or
 * the more complex way by using 'getSafe'. The 'getSafe' function can be used
 * to query cells outside the field ('get' would (hopefully) crash). This means
 * that 'getSafe' can be used to sample neighboring cells without the risk of
 * ending up outside the valid data buffer range.
 *
 */
template <typename T> class Field : public FieldBase {
public:
  /// Construct field
  Field(u32 width, u32 height, u32 depth, f32 cellSize = 1.0f)
      : FieldBase(width, height, depth, cellSize) {
    m_data = new T[m_cellCount];
  }

  /// Destruct field by freeing data
  ~Field() { delete m_data; }

  /// \copydoc FieldBase::paint
  void paint(Painter &painter, const Vec3F &offset,
             const Vec3F &padding) const override {
    paintT(painter, offset, padding);
  }

  /// Virtual method that can be overridden to paint an object of the type that
  /// is stored in the field
  virtual void paintT(Painter &painter, const Vec3F &offset = Vec3F(0, 0, 0),
                      const Vec3F &padding = Vec3F(0, 0, 0)) const = 0;

  /* Returns the reference to a vector in the vector field */
  T &get(u32 offset) {
    assert(offset < m_cellCount && "Offset cannot lie outside of field data");
    return m_data[offset];
  }

  /* Returns the reference to a vector in the vector field */
  const T &get(u32 offset) const {
    assert(offset < m_cellCount && "Offset cannot lie outside of field data");
    return m_data[offset];
  }

  /* Returns the reference to a vector in the vector field */
  T &get(s32 x, s32 y, s32 z) {
    const u32 offset = x + (m_dim.width * y) + (m_dim.width * m_dim.height * z);
    return get(offset);
  }

  /* Returns the reference to a vector in the vector field */
  const T &get(s32 x, s32 y, s32 z) const {
    const u32 offset = x + (m_dim.width * y) + (m_dim.width * m_dim.height * z);
    return get(offset);
  }

  /// Returns a reference to the object in the field at the specified position
  /// (x, y, z). The position is clamped to be valid in the field.
  T &getClamped(s32 x, s32 y, s32 z) {
    return get(clamp(x, 0, s32(m_dim.width) - 1),
               clamp(y, 0, s32(m_dim.height) - 1),
               clamp(z, 0, s32(m_dim.depth) - 1));
  }

  /// Returns a reference to the object in the field at the specified position
  /// (x, y, z). The position is clamped to be valid in the field.
  const T &getClamped(s32 x, s32 y, s32 z) const {
    return get(clamp(x, 0, s32(m_dim.width) - 1),
               clamp(y, 0, s32(m_dim.height) - 1),
               clamp(z, 0, s32(m_dim.depth) - 1));
  }

public:
  /// Swap the data of two field.
  /// \pre Fields must have the same dimensions
  static void swap(Field &field0, Field &field1) {
    assert(field0.m_dim.width == field1.m_dim.width &&
           field0.m_dim.height == field1.m_dim.height &&
           field0.m_dim.depth == field1.m_dim.depth &&
           "Swapping field data requires the fields to be of the same size");
    T *tmp = field0.m_data;
    field0.m_data = field1.m_data;
    field1.m_data = tmp;
  }

  /// Swap the data of two field.
  /// \pre Fields must have the same dimensions
  static void swap(Field *field0, Field *field1) {
    assert(field0->m_dim.width == field1->m_dim.width &&
           field0->m_dim.height == field1->m_dim.height &&
           field0->m_dim.depth == field1->m_dim.depth &&
           "Swapping field data requires the fields to be of the same size");
    T *tmp = field0->m_data;
    field0->m_data = field1->m_data;
    field1->m_data = tmp;
  }

protected:
  /// Field data. Laid out linearly
  T *m_data = nullptr;
};

} // namespace wind
