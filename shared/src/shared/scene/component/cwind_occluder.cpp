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

#include "cwind_occluder.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/scene/builder.hpp"
#include "shared/wind/wind_system.hpp"

#include <Components/BsCCollider.h>
#include <Physics/BsCollider.h>

// ========================================================================== //
// CSpline Implementation
// ========================================================================== //

namespace wind {

CWindOccluder::CWindOccluder(const bs::HSceneObject &parent, const Cube &cube)
    : Component(parent), m_volumeKind(VolumeKind::kCube), m_data(cube) {
  // Debug object
  const bs::HSceneObject obj =
      ObjectBuilder(ObjectType::kCube)
          .withName("windOccluderObj")
          .withSave(false)
          .withMaterial(ObjectBuilder::ShaderKind::kTransparentNoCull,
                        "res/textures/white.png", Vec2F::ONE,
                        Vec4F(0.0f, 0.357f, 0.588f, 0.2f))
          .withScale(cube.dim)
          .withCollider()
          .build();
  obj->setParent(SO());

  // Collision volume
  bs::HCollider collider = obj->getComponent<bs::CCollider>();
  collider->setIsTrigger(true);
  collider->setCollisionReportMode(bs::CollisionReportMode::Report);
  collider->setLayer(WindSystem::kWindOccluerLayer);
}

// -------------------------------------------------------------------------- //

CWindOccluder::CWindOccluder(const bs::HSceneObject &parent,
                             const Cylinder &cylinder)
    : Component(parent), m_volumeKind(VolumeKind::kCylinder), m_data(cylinder) {
  // Debug object
  const bs::HSceneObject obj =
      ObjectBuilder(ObjectType::kCylinder)
          .withName("windOccluderObj")
          .withSave(false)
          .withMaterial(ObjectBuilder::ShaderKind::kTransparentNoCull,
                        "res/textures/white.png", Vec2F::ONE,
                        Vec4F(0.0f, 0.357f, 0.588f, 0.2f))
          .withScale(Vec3F(cylinder.radius, cylinder.height, cylinder.radius))
          .withCollider()
          .build();
  obj->setParent(SO());

  // Collision volume
  bs::HCollider collider = obj->getComponent<bs::CCollider>();
  collider->setIsTrigger(true);
  collider->setCollisionReportMode(bs::CollisionReportMode::Report);
  collider->setLayer(WindSystem::kWindOccluerLayer);
}

// -------------------------------------------------------------------------- //

bs::RTTITypeBase *CWindOccluder::getRTTIStatic() {
  return CWindOccluderRTTI::instance();
}

} // namespace wind