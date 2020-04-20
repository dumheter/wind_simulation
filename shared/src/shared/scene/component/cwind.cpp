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
#include "shared/debug/debug_manager.hpp"
#include "shared/render/color.hpp"
#include "shared/scene/builder.hpp"
#include "shared/scene/component/cnet_component.hpp"

#include <Components/BsCMeshCollider.h>
#include <Physics/BsPhysics.h>
#include <Scene/BsSceneManager.h>
#include <microprofile/microprofile.h>

// ========================================================================== //
// CWind Implementation
// ========================================================================== //

namespace wind {

CWind::CWind(const bs::HSceneObject &parent, WindSystem::VolumeType volumeType,
             Vec3F pos, Vec3F scale)
    : Component(parent), m_volumeType(volumeType), m_pos(pos), m_scale(scale) {
  setName("WindComponent");
  mNotifyFlags = bs::TCF_Transform;

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
                .withPainter([this, pos, scale](auto &painter) {
                  MICROPROFILE_SCOPEI("CWind", "onPaint", MP_ORANGE2);
                  if (DebugManager::getBool("WVArrows")) {
                    constexpr Color arrowColor{255, 255, 0};
                    painter.setColor(arrowColor);
                    auto t = SO()->getTransform();
                    t.moveRelative(pos);
                    const auto s = t.getScale() * scale;
                    const Vec3F origo = t.pos();
                    const Vec3F a = origo - s / 2.0f;
                    if (!m_filipFlag) {
                      m_filipFlag = kBakeDone;
                      bakeDebugArrows(pos, scale);
                    }
                    for (u32 i = 0; i < m_cachedLines.size(); i += 2) {
                      const Vec3F start = m_cachedLines[i] + a;
                      const Vec3F end = m_cachedLines[i + 1] + a;
                      painter.drawLine(start, end);
                    }

                    painter.setColor(Color{255, 0, 0});
                    painter.drawArrow(origo, Vec3F(1.0f, 0.0f, 0.0f), 0.5f);
                    painter.setColor(Color{0, 255, 0});
                    painter.drawArrow(origo, Vec3F(0.0f, 1.0f, 0.0f), 0.5f);
                    painter.setColor(Color{0, 0, 255});
                    painter.drawArrow(origo, Vec3F(0.0f, 0.0f, 1.0f), 0.5f);
                  }
                })
                .build();
  so->setParent(parent);
}

void CWind::onCreated() {}

// -------------------------------------------------------------------------- //

void CWind::addFunction(BaseFn function) {
  m_functions.emplace_back(std::move(function));
  m_filipFlag = kRebake;
}

// -------------------------------------------------------------------------- //

void CWind::addFunctions(const std::vector<BaseFn> &functions) {
  for (const auto &function : functions) {
    addFunction(function);
  }
}

void CWind::onTransformChanged(bs::TransformChangedFlags flags) {
  m_filipFlag = kRebake;
}

Vec3F CWind::getWindAtPoint(Vec3F pos) const {
  const auto origo = SO()->getTransform().pos();
  pos = pos - origo;
  Vec3F wind = Vec3F::ZERO;
  for (auto fn : m_functions) {
    wind += fn(pos);
    if (std::isnan(wind.length())) {
      int a = 0;
      fn(pos);
    }
  }
  bs::Transform t = SO()->getTransform();
  t.setScale(bs::Vector3::ONE);
  t.setPosition(bs::Vector3::ZERO);

  return t.getMatrix().multiply(wind);
}

void CWind::bakeDebugArrows(Vec3F pos, Vec3F scale) {
  MICROPROFILE_SCOPEI("CWind", "bakeDebugArrows", MP_ORANGE1);
  m_cachedLines.clear();
  auto t = SO()->getTransform();
  t.moveRelative(pos);
  const auto s = t.getScale() * scale;
  const Vec3F origo = t.pos();
  const Vec3F a = origo - s / 2.0f;
  const s32 count =
      dclamp<s32>(static_cast<s32>((s.x + s.y + s.z) / 3.0f), 2, 8);
  const f32 dx = s.x / (count - 1);
  const f32 dy = s.y / (count - 1);
  const f32 dz = s.z / (count - 1);
  constexpr f32 arrowScale = 0.5f;
  constexpr f32 arrowHeadScale = 0.5f;
  const bs::SPtr<bs::PhysicsScene> &physicsScene =
      bs::gSceneManager().getMainScene()->getPhysicsScene();
#if 1
  for (s32 x = 0; x < count; ++x) {
    for (s32 y = 0; y < count; ++y) {
      for (s32 z = 0; z < count; ++z) {
        const Vec3F point = Vec3F{dx * x, dy * y, dz * z};
        const Vec3F pointR = a + point;
        constexpr f32 r = 0.001f;
        const bs::Sphere sphere{pointR, r};
        if (physicsScene->sphereOverlapAny(sphere,
                                           WindSystem::kWindVolumeLayer)) {
          Painter::buildArrow(m_cachedLines, point,
                              getWindAtPoint(pointR) * arrowScale,
                              arrowHeadScale);
        }
      }
    }
  }
#else
  /// FOR MAKING NICE GRAPHICS FOR PAPER
  /// draw arrows like vector_field.cpp::paintWithObstr
  constexpr f32 cellsize = 0.5f;
  constexpr f32 k = cellsize / 2.0f;
  const Vec3F padding{k, k, k};
  for (s32 z = 0; z < 29; ++z) {
    const f32 zPos = k + (z * cellsize);
    for (s32 y = 0; y < 13; ++y) {
      const f32 yPos = k + (y * cellsize);
      for (s32 x = 0; x < 29; ++x) {
        const f32 xPos = k + (x * cellsize);
        const Vec3F point{xPos, yPos, zPos};
        const Vec3F pointR = a + point;
        constexpr f32 r = 0.001f;
        const bs::Sphere sphere{pointR, r};
        if (physicsScene->sphereOverlapAny(sphere,
                                           WindSystem::kWindVolumeLayer)) {
          Painter::buildArrow(m_cachedLines, point,
                              getWindAtPoint(pointR) * arrowScale,
                              arrowHeadScale);
        }
      }
    }
  }
#endif
}

// -------------------------------------------------------------------------- //

bs::RTTITypeBase *CWind::getRTTIStatic() { return CWindSourceRTTI::instance(); }

bs::RTTITypeBase *CWind::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
