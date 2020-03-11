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

#include "obstruction_field.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <Components/BsCRenderable.h>
#include <CoreThread/BsCoreThread.h>
#include <Debug/BsDebugDraw.h>
#include <Physics/BsPhysics.h>

// ========================================================================== //
// VectorField Implementation
// ========================================================================== //

namespace wind {

ObstructionField::ObstructionField(u32 width, u32 height, u32 depth,
                                   f32 cellsize)
    : Field(width, height, depth, cellsize) {
  using namespace bs;
  for (u32 i = 0; i < m_dataSize; i++) {
    m_data[i] = false;
  }
}

// -------------------------------------------------------------------------- //

void ObstructionField::buildForScene(
    const bs::SPtr<bs::SceneInstance> &scene,
    const bs::Vector3 &position /*= bs::Vector3()*/) {
  using namespace bs;

  // Physics scene
  const SPtr<PhysicsScene> physicsScene = scene->getPhysicsScene();

  // Check for collisions in each cell
  for (u32 z = 0; z < m_dim.depth; z++) {
    f32 zPos = position.z + (z * m_cellSize);
    for (u32 y = 0; y < m_dim.height; y++) {
      f32 yPos = position.y + (y * m_cellSize);
      for (u32 x = 0; x < m_dim.width; x++) {
        f32 xPos = position.x + (x * m_cellSize);
        f32 offMin = 0.05f * m_cellSize;
        f32 offMax = 0.95f * m_cellSize;
        AABox aabb{Vector3(xPos + offMin, yPos + offMin, zPos + offMin),
                   Vector3(xPos + offMax, yPos + offMax, zPos + offMax)};
        if (physicsScene->boxOverlapAny(aabb, Quaternion::IDENTITY)) {
          get(x, y, z) = true;
        }
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void ObstructionField::debugDrawObject(const Vec3F &offset,
                                       const Vec3F &padding) {
  bs::DebugDraw::instance().setColor(bs::Color::BansheeOrange);

  u32 xPad = u32(padding.x);
  u32 yPad = u32(padding.y);
  u32 zPad = u32(padding.z);

  // Draw obstructions
  for (u32 z = zPad; z < m_dim.depth - zPad; z++) {
    const f32 zPos = offset.z + (z * m_cellSize);
    for (u32 y = yPad; y < m_dim.height - yPad; y++) {
      const f32 yPos = offset.y + (y * m_cellSize);
      for (u32 x = xPad; x < m_dim.width - xPad; x++) {
        const f32 xPos = offset.x + (x * m_cellSize);
        const Vec3F base =
            Vec3F(xPos + (m_cellSize / 2.0f), yPos + (m_cellSize / 2.0f),
                  zPos + (m_cellSize / 2.0f)) -
            (padding * m_cellSize);
        const bool obstructed = get(x, y, z);
        if (obstructed) {
          bs::DebugDraw::instance().drawCube(base, bs::Vector3::ONE *
                                                       (m_cellSize * 0.05f));
        }
      }
    }
  }
}

} // namespace wind
