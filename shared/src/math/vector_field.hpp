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

#include "common.hpp"
#include "math/field.hpp"

#include <Math/BsVector3.h>

// ========================================================================== //
// VectorField Declaration
// ========================================================================== //

namespace wind {

/* Class that represents a vector field */

class VectorField : public Field<bs::Vector3> {
public:
  enum class BorderKind {
    /* Vectors are set to a default value outside the field */
    DEFAULT,
    /* Vectors have their component going out the side zeroed. Meaning vectors
     * never point outside */
    CONTAINED,
    /* Vectors are zero outside the bounds */
    BLOCKED

  };

public:
  /* Construct a vector-field with the specified 'width', 'height' and
   * 'depth' (in number of cells). The size of a cell (in meters) can also be
   * specified.  */
  VectorField(u32 width, u32 height, u32 depth, f32 cellsize = 1.0f);

  /* \copydoc Field::debugDrawObject */
  void debugDrawObject(const bs::Vector3 &offset = bs::Vector3()) override;

  /* \copydoc Field::getSafe */
  bs::Vector3 getSafe(s32 x, s32 y, s32 z) override;

  /* Set the kind of border condition to use */
  void setBorder(BorderKind kind);

  /* Set the default value to use when the border kind is 'DEFAULT' */
  void setBorderDefaultValue(const bs::Vector3 &value);

private:
  /* Current border kind */
  BorderKind m_borderKind = BorderKind::CONTAINED;
  /* Border value for 'DEFAULT' border kind */
  bs::Vector3 m_borderDefaultValue = bs::Vector3::ZERO;
};

} // namespace wind