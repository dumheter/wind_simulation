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

#include "cwind.hpp"
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
// CWind Implementation
// ========================================================================== //

namespace wind {

CWind::CWind(const bs::HSceneObject &parent, WindSystem::VolumeType volumeType)
    : Component(parent) {
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
                .withTriggerCollider(colliderType, WindSystem::kWindVolumeLayer)
                .build();
  so->setParent(parent);
  setupColl(so->getComponent<bs::CCollider>());
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

// -------------------------------------------------------------------------- //

bs::Vector3 CWind::getWindForce(bs::Vector3 pos) const {
  bs::Vector3 force{bs::Vector3::ZERO};
  for (auto fn : m_functions) {
    force += fn(pos);
  }
  return force;
}

// -------------------------------------------------------------------------- //

void CWind::fixedUpdate() {
  if (!m_collisions.empty()) {
    const f32 dt = bs::gTime().getFixedFrameDelta();
    for (auto &collision : m_collisions) {
      if (collision.netComp) {
        bs::Vector3 force =
            dt * getWindForce(collision.netComp->getState().position);
        bs::Transform t = SO()->getTransform();
        t.setScale(bs::Vector3::ONE);
        t.setPosition(bs::Vector3::ZERO);
        auto newforce = t.getMatrix().multiply(force);
        collision.netComp->addForce(newforce, bs::ForceMode::Velocity);
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void CWind::onCreated() {}

// -------------------------------------------------------------------------- //

bs::RTTITypeBase *CWind::getRTTIStatic() { return CWindSourceRTTI::instance(); }

bs::RTTITypeBase *CWind::getRTTI() const { return getRTTIStatic(); }

// -------------------------------------------------------------------------- //

void CWind::setupColl(const bs::HCollider &collider) {
  collider->onCollisionBegin.connect([this](const bs::CollisionData &data) {
    if (data.collider[1] == nullptr) {
      return;
    }
    bs::HSceneObject so = data.collider[1]->SO();
    const HCNetComponent netComp = so->getComponent<CNetComponent>();
    if (netComp) {
      m_collisions.emplace_back(Collision{netComp->getUniqueId(), netComp});
    } else {
      logWarning("[windSource] col with non net component");
    }
  });

  collider->onCollisionEnd.connect([this](const bs::CollisionData &data) {
    if (data.collider[1] == nullptr) {
      return;
    }
    bs::HSceneObject so = data.collider[1]->SO();
    const HCNetComponent netComp = so->getComponent<CNetComponent>();
    if (netComp) {
      const UniqueId id = netComp->getUniqueId();
      for (auto it = m_collisions.begin(); it < m_collisions.end(); ++it) {
        if (it->id == id) {
          m_collisions.erase(it);
          break;
        }
      }
    }
  });
}

} // namespace wind
