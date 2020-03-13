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

#include "shared/scene/rtti.hpp"

#include "shared/log.hpp"
#include "shared/math/math.hpp"
#include "shared/scene/component_handles.hpp"
#include "shared/utility/unique_id.hpp"
#include "shared/wind/base_functions.hpp"
#include "shared/wind/wind_system.hpp"

#include <Material/BsMaterial.h>
#include <RTTI/BsMathRTTI.h>
#include <Reflection/BsRTTIPlain.h>
#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// CWindSource Declaration
// ========================================================================== //

namespace wind {

struct Collision {
  UniqueId id;
  HCNetComponent netComp;
};

///
class CWindSource : public bs::Component {
  friend class CWindSourceRTTI;

public:
  /// Default constructor required for serialization
  CWindSource() = default;

  /// @size Size of the cube volume where wind interacts.
  CWindSource(const bs::HSceneObject &parent,
              WindSystem::VolumeType volumeType,
              Vec3F offset = Vec3F::ZERO);

  void addFunction(BaseFn function);

  void addFunctions(const std::vector<BaseFn> &functions);

  bs::Vector3 getWindForce(bs::Vector3 pos) const;

  void fixedUpdate() override;

  void onCreated() override;

  void onCollision();

  const std::vector<BaseFn> &getFunctions() const { return m_functions; }

  std::vector<BaseFn> &getFunctions() { return m_functions; }

  WindSystem::VolumeType getVolumeType() const { return m_volumeType; }

  Vec3F getOffset() const { return m_offset; }

  static bs::RTTITypeBase *getRTTIStatic();

  bs::RTTITypeBase *getRTTI() const override;

private:
  template <typename T> void setupCollision(const T &collider);

private:
  std::vector<BaseFn> m_functions{};
  std::vector<Collision> m_collisions{};
  /// Used when saving the wind source
  WindSystem::VolumeType m_volumeType = WindSystem::VolumeType::kCube;
  /// Used when saving the wind source
  Vec3F m_offset = Vec3F::ZERO;
};

// -------------------------------------------------------------------------- //

/// CWindSource handle type
using HCWindSource = bs::GameObjectHandle<CWindSource>;

template <typename T> void CWindSource::setupCollision(const T &collider) {
  collider->onCollisionBegin.connect([this](const bs::CollisionData &data) {
    if (data.collider[1] == nullptr) {
      return;
    }
    auto so = data.collider[1]->SO();
    HCNetComponent netComp = so->getComponent<CNetComponent>();
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
    }
  });
}

} // namespace wind

// ========================================================================== //
// CWindSourceRTTI
// ========================================================================== //

namespace wind {

///
class CWindSourceRTTI
    : public bs::RTTIType<CWindSource, bs::Component, CWindSourceRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_END_RTTI_MEMBERS

public:
  const bs::String &getRTTIName() override {
    static bs::String name = "CNetComponent";
    return name;
  }

  bs::UINT32 getRTTIId() override { return TID_CWindSource; }

  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CWindSource>();
  }
};

} // namespace wind
