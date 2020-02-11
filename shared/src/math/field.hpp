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

#include "math/dim.hpp"
#include "types.hpp"

#include <BsPrerequisites.h>
#include <Debug/BsDebugDraw.h>

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
template <typename T> class Field {
public:
  /* Field position */
  struct Pos {
    s32 x, y, z;
  };

public:
  /* */
  Field(u32 width, u32 height, u32 depth, f32 cellSize = 1.0f);

  /* */
  ~Field();

  /* Virtual function responsible for drawing the objects in the field */
  virtual void debugDrawObject(const bs::Vector3 &offset = bs::Vector3(0, 0,
                                                                       0)) = 0;

  /* Draw lines to help with debugging the field. This is called only once to
   * produce all the lines required for the 'DebugDraw' class. After
   * clearing 'DebugDraw' this function needs to be called again */
  void debugDraw(const bs::Vector3 &offset = bs::Vector3(0, 0, 0),
                 bool drawFrame = true);

  /* Convert position (x, y, z) to a data offset */
  u32 fromPos(s32 x, s32 y, s32 z);

  /* Convert position (x, y, z) to a data offset */
  u32 fromPos(const Pos &pos) { return fromPos(pos.x, pos.y, pos.z); }

  /* Convert offset in data to position */
  Pos fromOffset(u32 offset);

  /* Returns whether or not the specified position is an edge position. i.e. it
   * lies on the edge of the field */
  bool isEdgePos(s32 x, s32 y, s32 z) const;

  /* Returns whether or not the specified position is an edge position. i.e. it
   * lies on the edge of the field */
  bool isEdgePos(const Pos &pos) const {
    return isEdgePos(pos.x, pos.y, pos.z);
  }

  /* Returns whether or not a position is in bounds of the field */
  bool isInBounds(s32 x, s32 y, s32 z) const;

  /* Returns whether or not a position is in bounds of the field */
  bool isInBounds(const Pos &pos) const {
    return isInBounds(pos.x, pos.y, pos.z);
  }

  /* Returns whether or not a position is in the X bounds of the field */
  bool isInBoundsX(s32 x) const;

  /* Returns whether or not a position is in the Y bounds of the field */
  bool isInBoundsY(s32 y) const;

  /* Returns whether or not a position is in the X bounds of the field */
  bool isInBoundsZ(s32 z) const;

  /* Returns the reference to a vector in the vector field */
  T &get(u32 offset);

  /* Returns the reference to a vector in the vector field */
  const T &get(u32 offset) const;

  /* Returns the reference to a vector in the vector field */
  T &get(s32 x, s32 y, s32 z);

  /* Returns the reference to a vector in the vector field */
  const T &get(s32 x, s32 y, s32 z) const;

  /* Returns the value at a position. Safety are added such that points outside
   * the field returns a special value. This value depends on the type of
   * field */
  virtual T getSafe(s32 x, s32 y, s32 z) = 0;

  /* Returns the value at a position. Safety are added such that points outside
   * the field returns a special value. This value depends on the type of
   * field */
  T getSafe(const Pos &pos) { return getSafe(pos.x, pos.y, pos.z); }

  /* Returns the size of the data buffer in number of elements */
  u32 getDataSize() { return m_dataSize; }

  /* Returns the dimensions of the field */
  const Dim3D &getDim() { return m_dim; }

public:
  /* Swap the data of two fields. This requires the fields to be of the same
   * dimension */
  static void swap(Field &field0, Field &field1);

protected:
  /* Dimensions */
  Dim3D m_dim;
  /* Cell size in meters */
  f32 m_cellSize;

  /* Field data. Laid out linearly */
  T *m_data = nullptr;
  /* Size of data (in number of elements) */
  u32 m_dataSize = 0;
};

} // namespace wind

// ========================================================================== //
// Field Implementation
// ========================================================================== //

namespace wind {

template <typename T>
Field<T>::Field(u32 width, u32 height, u32 depth, f32 cellSize)
    : m_dim(Dim3D{width, height, depth}), m_cellSize(cellSize) {
  using namespace bs;
  m_dataSize = m_dim.width * m_dim.height * m_dim.depth;
  m_data = new T[m_dataSize];
}

// -------------------------------------------------------------------------- //

template <typename T> inline Field<T>::~Field() { delete m_data; }

// -------------------------------------------------------------------------- //

template <typename T>
void Field<T>::debugDraw(const bs::Vector3 &offset, bool drawFrame) {
  debugDrawObject(offset);

  // Draw frame
  if (drawFrame) {
    bs::Vector<bs::Vector3> points;

    points.clear();

    // Draw lines parallell with the x-axis
    const f32 xStart = offset.x;
    const f32 xEnd = xStart + (m_cellSize * m_dim.width);
    for (u32 z = 0; z < m_dim.depth + 1; z++) {
      const f32 zPos = offset.z + (z * m_cellSize);
      for (u32 y = 0; y < m_dim.height + 1; y++) {
        const f32 yPos = offset.y + (y * m_cellSize);
        points.push_back(bs::Vector3(xStart, yPos, zPos));
        points.push_back(bs::Vector3(xEnd, yPos, zPos));
      }
    }

    // Draw lines parallell with the x-axis
    const f32 yStart = offset.y;
    const f32 yEnd = yStart + (m_cellSize * m_dim.height);
    for (u32 z = 0; z < m_dim.depth + 1; z++) {
      const f32 zPos = offset.z + (z * m_cellSize);
      for (u32 x = 0; x < m_dim.width + 1; x++) {
        const f32 xPos = offset.x + (x * m_cellSize);
        points.push_back(bs::Vector3(xPos, yStart, zPos));
        points.push_back(bs::Vector3(xPos, yEnd, zPos));
      }
    }

    // Draw lines parallell with the x-axis
    const f32 zStart = offset.z;
    const f32 zEnd = zStart + (m_cellSize * m_dim.depth);
    for (u32 y = 0; y < m_dim.height + 1; y++) {
      const f32 yPos = offset.y + (y * m_cellSize);
      for (u32 x = 0; x < m_dim.width + 1; x++) {
        const f32 xPos = offset.x + (x * m_cellSize);
        points.push_back(bs::Vector3(xPos, yPos, zStart));
        points.push_back(bs::Vector3(xPos, yPos, zEnd));
      }
    }

    bs::DebugDraw::instance().setColor(bs::Color::White);
    bs::DebugDraw::instance().drawLineList(points);
  }
}

// -------------------------------------------------------------------------- //

template <typename T> inline u32 Field<T>::fromPos(s32 x, s32 y, s32 z) {
  assert(isInBounds(x, y, z) && "Position cannot lie outside of field");
  return x + (m_dim.width * y) + (m_dim.width * m_dim.height * z);
}

// -------------------------------------------------------------------------- //

template <typename T>
inline typename Field<T>::Pos Field<T>::fromOffset(u32 offset) {
  assert(offset < m_dataSize && "Offset cannot lie outside of field data");
  s32 x = offset % m_dim.width;
  s32 y = (offset % (m_dim.width * m_dim.height)) / m_dim.width;
  s32 z = offset / (m_dim.width * m_dim.height);
  return Pos{x, y, z};
}

// -------------------------------------------------------------------------- //

template <typename T>
inline bool Field<T>::isEdgePos(s32 x, s32 y, s32 z) const {
  assert(isInBounds(x, y, z) && "Position cannot lie outside of field");
  return x == 0 || y == 0 || z == 0 || x == m_dim.width - 1 ||
         y == m_dim.height - 1 || z == m_dim.depth - 1;
}

// -------------------------------------------------------------------------- //

template <typename T>
inline bool Field<T>::isInBounds(s32 x, s32 y, s32 z) const {
  return isInBoundsX(x) && isInBoundsY(y) && isInBoundsZ(z);
}

// -------------------------------------------------------------------------- //

template <typename T> inline bool Field<T>::isInBoundsX(s32 x) const {
  return x >= 0 && x < s32(m_dim.width);
}

// -------------------------------------------------------------------------- //

template <typename T> inline bool Field<T>::isInBoundsY(s32 y) const {
  return y >= 0 && y < s32(m_dim.height);
}

// -------------------------------------------------------------------------- //

template <typename T> inline bool Field<T>::isInBoundsZ(s32 z) const {
  return z >= 0 && z < s32(m_dim.depth);
}

// -------------------------------------------------------------------------- //

template <typename T> inline T &Field<T>::get(u32 offset) {
  assert(offset < m_dataSize && "Offset cannot lie outside of field data");
  return m_data[offset];
}

// -------------------------------------------------------------------------- //

template <typename T> inline const T &Field<T>::get(u32 offset) const {
  assert(offset < m_dataSize && "Offset cannot lie outside of field data");
  return m_data[offset];
}

// -------------------------------------------------------------------------- //

template <typename T> T &Field<T>::get(s32 x, s32 y, s32 z) {
  const u32 offset = x + (m_dim.width * y) + (m_dim.width * m_dim.height * z);
  return get(offset);
}

// -------------------------------------------------------------------------- //

template <typename T> const T &Field<T>::get(s32 x, s32 y, s32 z) const {
  const u32 offset = x + (m_dim.width * y) + (m_dim.width * m_dim.height * z);
  return get(offset);
}

// -------------------------------------------------------------------------- //

template <typename T> inline void Field<T>::swap(Field &field0, Field &field1) {
  assert(field0.m_dim.width == field1.m_dim.width &&
         field0.m_dim.height == field1.m_dim.height &&
         field0.m_dim.depth == field1.m_dim.depth &&
         "Swapping field data requires the fields to be of the same size");
  bs::Vector3 *tmp = field0.m_field;
  field0.m_field = field1.m_field;
  field1.m_field = tmp;
}

} // namespace wind