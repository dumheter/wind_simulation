#include "cwind_affectable.hpp"
#include "shared/wind/wind_system.hpp"
#include <Components/BsCRigidbody.h>
#include <limits>

namespace wind {

CWindAffectable::CWindAffectable(const bs::HSceneObject &parent)
    : bs::Component(parent) {
  setName("WindAffectable");
}

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

bs::RTTITypeBase *CWindAffectable::getRTTIStatic() {
  return CWindAffectableRTTI::instance();
}

bs::RTTITypeBase *CWindAffectable::getRTTI() const { return getRTTIStatic(); }

}
