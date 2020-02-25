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

#include "wind_src.hpp"
#include "Components/BsCBoxCollider.h"
#include "Components/BsCRenderable.h"
#include "Scene/BsSceneObject.h"
#include "Utility/BsTime.h"
#include "log.hpp"
#include "scene/cnet_component.hpp"
#include "utility/bsprinter.hpp"

// ========================================================================== //
// CWindSource Implementation
// ========================================================================== //

namespace wind {

CWindSource::CWindSource(const bs::HSceneObject &parent,
                         const bs::HMaterial &mat, const bs::HMesh &mesh)
    : bs::Component(parent) {
  setName("WindComponent");
  auto so = bs::SceneObject::create("CWindSource");
  so->setParent(parent);

  { // TODO @filip
    auto render = so->addComponent<bs::CRenderable>();
    render->setMesh(mesh);
    render->setMaterial(mat);
  }

  auto collider = so->addComponent<bs::CBoxCollider>();
  collider->setIsTrigger(true);
  collider->setCollisionReportMode(bs::CollisionReportMode::Report);

  collider->onCollisionBegin.connect([this](const bs::CollisionData &data) {
    if (data.collider[1] == nullptr) {
      return;
    }
    auto so = data.collider[1]->SO();
    HCNetComponent netComp = so->getComponent<CNetComponent>();
    if (netComp) {
      logVerbose("[windSource] col begin with {}, #{}",
                 so->getRTTI()->getRTTIName(), netComp->getUniqueId().raw());
      m_collisions.emplace_back(Collision{netComp->getUniqueId(), netComp});
    } else {
      logWarning("[windSource] col with non net component");
    }
  });

  collider->onCollisionEnd.connect([this](const bs::CollisionData &data) {
    if (data.collider[1] == nullptr) {
      return;
    }
    auto so = data.collider[1]->SO();
    auto netComp = so->getComponent<CNetComponent>();
    if (netComp) {
      const auto id = netComp->getUniqueId();
      for (auto it = m_collisions.begin(); it < m_collisions.end(); ++it) {
        if (it->id == id) {
          m_collisions.erase(it);
          break;
        }
      }
      logVerbose("[windSource] col end with {}, #{}, {} left",
                 so->getRTTI()->getRTTIName(), id.raw(), m_collisions.size());
    }
  });
}

void CWindSource::addFunction(BaseFn function) {
  m_functions.emplace_back(std::move(function));
}

bs::Vector3 CWindSource::getWindForce(bs::Vector3 pos) const {
  bs::Vector3 force{bs::Vector3::ZERO};
  for (auto fn : m_functions) {
    force += fn(pos);
  }
  return force;
}

void CWindSource::fixedUpdate() {
  if (!m_collisions.empty()) {
    const f32 dt = bs::gTime().getFixedFrameDelta();
    for (auto &collision : m_collisions) {
      collision.netComp->addForce(
          dt * getWindForce(collision.netComp->getState().getPosition()),
          bs::ForceMode::Velocity);
    }
  }
}

// -------------------------------------------------------------------------- //

void CWindSource::onCreated() {}

// -------------------------------------------------------------------------- //

bs::RTTITypeBase *CWindSource::getRTTIStatic() {
  return CWindSourceRTTI::instance();
}

bs::RTTITypeBase *CWindSource::getRTTI() const { return getRTTIStatic(); }


} // namespace wind
