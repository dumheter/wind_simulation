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

#include "wind_system.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/scene/wind_src.hpp"
#include <Physics/BsPhysics.h>
#include <Scene/BsSceneManager.h>
#include <Components/BsCCollider.h>

// ========================================================================== //
// WindSystem Implementation
// ========================================================================== //

namespace wind {

String WindSystem::volumeTypeToString(VolumeType type) {
  switch (type) {
  case VolumeType::kCylinder: {
    return "cylinder";
  }
  case VolumeType::kCube:
    [[fallthrough]];
  default:
    return "cube";
  }
}

WindSystem::VolumeType WindSystem::stringToVolumeType(const String &type) {
  if (type == "cylinder") {
    return VolumeType::kCylinder;
  }
  return VolumeType::kCube;
}

u8 WindSystem::volumeTypeToU8(VolumeType type) {
  return static_cast<u8>(type);
}

WindSystem::VolumeType WindSystem::u8ToVolumeType(u8 type) {
  return static_cast<VolumeType>(type);
}

// ============================================================ //

WindSystem &WindSystem::instance() {
  static WindSystem windSystem{};
  return windSystem;
}

Vec3F WindSystem::getWindAtPoint(Vec3F point) const {
  Vec3F wind = Vec3F::ZERO;
  constexpr f32 r = 0.01f;
  const bs::Sphere sphere{point, r};
  const bs::SPtr<bs::PhysicsScene> &physicsScene =
	  bs::gSceneManager().getMainScene()->getPhysicsScene();
	
  const auto colliders = physicsScene->sphereOverlap(sphere, kWindVolumeLayer);
  for (const auto& collider : colliders) {
    bs::HSceneObject so = collider->SO()->getParent();
    const HCWindSource windSource = so->getComponent<CWindSource>();
    wind += windSource->getWindAtPoint(point);
  }

  return wind;
}

} // namespace wind
