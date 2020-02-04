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

#include "vector_field.hpp"

// ========================================================================== //
// VectorField Implementation
// ========================================================================== //

namespace wind
{

VectorField::VectorField(u32 width, u32 height, u32 depth, f32 cellsize)
    : m_width(width), m_height(height), m_depth(depth), m_cellSize(cellsize)
{
    using namespace bs;

    m_fieldSize = m_width * m_height * m_depth;
    m_field = new Vector3[m_fieldSize];
}

// -------------------------------------------------------------------------- //

VectorField::~VectorField()
{
    delete m_field;
}

// -------------------------------------------------------------------------- //

void VectorField::debugDraw()
{
}

// -------------------------------------------------------------------------- //

bs::Vector3 &VectorField::GetVector(u32 x, u32 y, u32 z)
{
    const u32 offset = x + (m_width * y) + (m_width * m_height * z);
    return m_field[offset];
}

// -------------------------------------------------------------------------- //

const bs::Vector3 &VectorField::GetVector(u32 x, u32 y, u32 z) const
{
    const u32 offset = x + (m_width * y) + (m_width * m_height * z);
    return m_field[offset];
}

} // namespace wind
