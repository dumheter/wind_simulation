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

#include "cwind_affectable.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/wind/wind_system.hpp"
#include <Components/BsCRigidbody.h>
#include <limits>

// ========================================================================== //
// CWindAffectable Implementation
// ========================================================================== //

namespace wind {

CWindAffectable::CWindAffectable(const bs::HSceneObject &parent)
    : bs::Component(parent) {
  setName("WindAffectable");
}

// -------------------------------------------------------------------------- //

void CWindAffectable::fixedUpdate() {
  const Vec3F pos = SO()->getTransform().pos();
  const Vec3F windAtPos = gWindSystem().getWindAtPoint(pos);
  if (std::abs(windAtPos.x) > std::numeric_limits<f32>::min() ||
      std::abs(windAtPos.y) > std::numeric_limits<f32>::min() ||
      std::abs(windAtPos.z) > std::numeric_limits<f32>::min()) {

    auto rigid = SO()->getComponent<bs::CRigidbody>();
    if (rigid && !rigid->getIsKinematic()) {
      const f32 dt = bs::gTime().getFixedFrameDelta();
      rigid->addForce(windAtPos * dt, bs::ForceMode::Velocity);
    }
  }
}

// -------------------------------------------------------------------------- //

bs::RTTITypeBase *CWindAffectable::getRTTIStatic() {
  return CWindAffectableRTTI::instance();
}

// -------------------------------------------------------------------------- //

bs::RTTITypeBase *CWindAffectable::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
