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

#include <BsPrerequisites.h>
#include <Components/BsCBoxCollider.h>
#include <Components/BsCRenderable.h>
#include <CoreThread/BsCoreThread.h>
#include <Debug/BsDebugDraw.h>
#include <Mesh/BsMesh.h>
#include <Physics/BsPhysics.h>
#include <Physics/BsPhysicsCommon.h>
#include <Physics/BsPhysicsMaterial.h>
#include <Scene/BsSceneObject.h>
#include <Threading/BsThreading.h>

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

void ObstructionField::debugDrawObject(const bs::Vector3 &offset) {
  bs::DebugDraw::instance().setColor(bs::Color::BansheeOrange);

  // Draw vectors
  for (u32 z = 0; z < m_dim.depth; z++) {
    const f32 zPos = offset.z + (z * m_cellSize);
    for (u32 y = 0; y < m_dim.height; y++) {
      const f32 yPos = offset.y + (y * m_cellSize);
      for (u32 x = 0; x < m_dim.width; x++) {
        const f32 xPos = offset.x + (x * m_cellSize);
        const bs::Vector3 base(xPos + (m_cellSize / 2.0f),
                               yPos + (m_cellSize / 2.0f),
                               zPos + (m_cellSize / 2.0f));
        const bool obstructed = get(x, y, z);

        if (obstructed) {
          bs::DebugDraw::instance().drawCube(base, bs::Vector3::ONE *
                                                       (m_cellSize * 0.05f));
        }
      }
    }
  }
}

// -------------------------------------------------------------------------- //

bool ObstructionField::getSafe(s32 x, s32 y, s32 z) {
  return isInBounds(x, y, z) ? get(x, y, z) : false;
}

// -------------------------------------------------------------------------- //

ObstructionField *
ObstructionField::buildForScene(const bs::SPtr<bs::SceneInstance> &scene,
                                const bs::Vector3 &extent,
                                const bs::Vector3 &position, f32 cellSize) {
  using namespace bs;

  // Physics scene
  const SPtr<PhysicsScene> physicsScene = scene->getPhysicsScene();

  // Dimensions
  f32 mul = 1 / cellSize;
  u32 width = u32(extent.x * mul);
  u32 height = u32(extent.y * mul);
  u32 depth = u32(extent.z * mul);

  // Check for collisions in each cell
  ObstructionField *field =
      new ObstructionField(width, height, depth, cellSize);
  for (u32 z = 0; z < depth; z++) {
    f32 zPos = position.z + (z * cellSize);
    for (u32 y = 0; y < height; y++) {
      f32 yPos = position.y + (y * cellSize);
      for (u32 x = 0; x < width; x++) {
        f32 xPos = position.x + (x * cellSize);
        f32 offMin = 0.05f * cellSize;
        f32 offMax = 0.95f * cellSize;
        AABox aabb{Vector3(xPos + offMin, yPos + offMin, zPos + offMin),
                   Vector3(xPos + offMax, yPos + offMax, zPos + offMax)};
        if (physicsScene->boxOverlapAny(aabb, Quaternion::IDENTITY)) {
          field->get(x, y, z) = true;
        }
      }
    }
  }
  return field;
}

} // namespace wind
