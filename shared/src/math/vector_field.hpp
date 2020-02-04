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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
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

#include <Math/BsVector3.h>

// ========================================================================== //
// VectorField Declaration
// ========================================================================== //

namespace wind
{

/* Class that represents a vector field */
class VectorField
{
  public:
    /* Construct a vector-field with the specified 'width', 'height' and
     * 'depth' (in number of cells). The size of a cell (in meters) can also be
     * specified.  */
    VectorField(u32 width, u32 height, u32 depth, f32 cellsize);

    /* Destruct vector-field */
    ~VectorField();

    /* Draw lines to help with debugging the vector field. This is called only
     * once to produce all the lines required for the 'DebugDraw' class. After
     * clearing 'DebugDraw' this function needs to be called again */
    void debugDraw(const bs::Vector3 &offset = bs::Vector3(0, 0, 0), bool drawFrame = true);

    /* Returns the reference to a vector in the vector field */
    bs::Vector3 &GetVector(u32 x, u32 y, u32 z);

    /* Returns the reference to a vector in the vector field */
    const bs::Vector3 &GetVector(u32 x, u32 y, u32 z) const;

  private:
    /* Dimensions */
    u32 m_width, m_height, m_depth;
    /* Cell size in meters */
    f32 m_cellSize;

    /* Vector field data. Laid out linearly */
    bs::Vector3 *m_field = nullptr;
    /* Size of field (in number of vectors) */
    u32 m_fieldSize = 0;
};

} // namespace wind