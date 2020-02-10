// MIT License
//
// Copyright (c) 2020 Filip Bj�rklund, Christoffer Gustafsson
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

#include <Scene/BsSceneManager.h>

// ========================================================================== //
// VectorField Declaration
// ========================================================================== //

namespace wind {

/* Class that represents a density field. This field represents density at
 * different discrete points in 3D-space */
class DensityField : public Field<f32> {
public:
  /* Construct an obstruction field with the specified 'width', 'height' and
   * 'depth' (in number of cells). The size of a cell (in meters) can also be
   * specified.  */
  DensityField(u32 width, u32 height, u32 depth, f32 cellsize = 1.0f);

  /* \copydoc Field::debugDrawObject */
  void debugDrawObject(const bs::Vector3 &offset = bs::Vector3()) override;

  /* \copydoc Field::getSafe */
  f32 getSafe(s32 x, s32 y, s32 z) override;
};

} // namespace wind