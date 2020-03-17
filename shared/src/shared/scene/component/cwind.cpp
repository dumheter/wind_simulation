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

#include "cwind.hpp"
#include "Components/BsCBoxCollider.h"
#include "Components/BsCRenderable.h"
#include "Scene/BsSceneObject.h"
#include "Utility/BsTime.h"
#include "shared/asset.hpp"
#include "shared/log.hpp"
#include "shared/scene/builder.hpp"
#include "shared/scene/component/cnet_component.hpp"

#include <Components/BsCMeshCollider.h>

// ========================================================================== //
// CWind Implementation
// ========================================================================== //

namespace wind {

CWind::CWind(const bs::HSceneObject &parent, WindSystem::VolumeType volumeType, Vec3F pos, Vec3F scale)
    : Component(parent), m_volumeType(volumeType), m_pos(pos), m_scale(scale) {
  setName("WindComponent");

  // Determine collider object type
  ObjectType colliderType = ObjectType::kCube;
  switch (volumeType) {
  case WindSystem::VolumeType::kCylinder: {
    colliderType = ObjectType::kCylinder;
    break;
  }
  case WindSystem::VolumeType::kCube:
    break;
  default:
    break;
  }

  // Build trigger collider
  auto so = ObjectBuilder(ObjectType::kEmpty)
                .withSave(false)
                .withPosition(pos)
                .withScale(scale)
                .withName("WindCollider")
                .withTriggerCollider(colliderType, WindSystem::kWindVolumeLayer)
                .build();
  so->setParent(parent);
}

// -------------------------------------------------------------------------- //

void CWind::addFunction(BaseFn function) {
  m_functions.emplace_back(std::move(function));
}

// -------------------------------------------------------------------------- //

void CWind::addFunctions(const std::vector<BaseFn> &functions) {
  for (const auto &function : functions) {
    addFunction(function);
  }
}

Vec3F CWind::getWindAtPoint(Vec3F pos) const {
  Vec3F wind = Vec3F::ZERO;
  for (auto fn : m_functions) {
    wind += fn(pos);
  }
  return wind;
}

// -------------------------------------------------------------------------- //

bs::RTTITypeBase *CWind::getRTTIStatic() { return CWindSourceRTTI::instance(); }

bs::RTTITypeBase *CWind::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
