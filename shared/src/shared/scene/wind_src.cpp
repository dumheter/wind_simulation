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

#include "wind_src.hpp"
#include "Components/BsCBoxCollider.h"
#include "Components/BsCRenderable.h"
#include "Scene/BsSceneObject.h"
#include "Utility/BsTime.h"
#include "shared/asset.hpp"
#include "shared/log.hpp"
#include "shared/scene/builder.hpp"
#include "shared/scene/cnet_component.hpp"

#include <Components/BsCMeshCollider.h>

// ========================================================================== //
// CWindSource Implementation
// ========================================================================== //

namespace wind {

CWindSource::CWindSource(const bs::HSceneObject &parent,
                         WindSystem::VolumeType volumeType,
                         Vec3F offset)
    : bs::Component(parent), m_volumeType(volumeType), m_offset(offset) {
  setName("WindComponent");
  auto so = ObjectBuilder(ObjectType::kEmpty)
            .withSave(false)
            .withName("WindSrcCollider")
            .build();
  so->setParent(parent);

  const auto setCommonSettings = [this](auto &collider) {
    collider->setIsTrigger(true);
    collider->setLayer(WindSystem::kWindVolumeLayer);
  };

  switch (volumeType) {
  case WindSystem::VolumeType::kCylinder: {
    logInfo("[CWindSource] cylinder volume");
    // TODO only load the physics mesh
    auto [_, pmesh] = Asset::loadMeshWithPhysics("res/meshes/cylinder.fbx",
                                                 1.0f, false, true);
    auto collider = so->addComponent<bs::CMeshCollider>();
    collider->setMesh(pmesh);
    setCommonSettings(collider);
    break;
  }
  case WindSystem::VolumeType::kCube:
    [[fallthrough]];
  default: {
    logInfo("[CWindSource] cube volume");
    auto collider = so->addComponent<bs::CBoxCollider>();
    setCommonSettings(collider);
  }
  }
}

void CWindSource::addFunction(BaseFn function) {
  m_functions.emplace_back(std::move(function));
}

void CWindSource::addFunctions(const std::vector<BaseFn> &functions) {
  for (const auto &function : functions) {
    addFunction(function);
  }
}

Vec3F CWindSource::getWindAtPoint(Vec3F pos) const {
  Vec3F wind = Vec3F::ZERO;
  for (auto fn : m_functions) {
    wind += fn(pos);
  }
  return wind;
}

// -------------------------------------------------------------------------- //

bs::RTTITypeBase *CWindSource::getRTTIStatic() {
  return CWindSourceRTTI::instance();
}

bs::RTTITypeBase *CWindSource::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
