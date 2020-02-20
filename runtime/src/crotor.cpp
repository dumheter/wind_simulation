#include "crotor.hpp"
#include "Components/BsCRigidbody.h"
#include "Math/BsDegree.h"
#include "Physics/BsRigidbody.h"
#include "Scene/BsSceneObject.h"
#include "alflib/core/assert.hpp"

namespace wind {

CRotor::CRotor(const bs::HSceneObject &parent)
    : bs::Component(parent), m_rotation() {
  setName("CRotor");
}

void CRotor::fixedUpdate() { SO()->yaw(bs::Degree(m_rotation)); }

void CRotor::setRotation(f32 degree) { m_rotation = degree; }

bs::RTTITypeBase *CRotor::getRTTIStatic() { return CRotorRTTI::instance(); }

bs::RTTITypeBase *CRotor::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
