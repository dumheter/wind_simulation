#include "crotor.hpp"
#include "shared/scene/component/cwind.hpp"

namespace wind {

CRotor::CRotor() : m_rotation() {}

CRotor::CRotor(const bs::HSceneObject &parent, Vec3F rotation)
    : bs::Component(parent), m_rotation(), m_inputRotation(rotation) {
  setName("CRotor");
  setRotation(rotation);
}

void CRotor::fixedUpdate() { SO()->rotate(m_rotation); }

void CRotor::setRotation(Vec3F rot) {
  m_rotation =
      bs::Quaternion(bs::Degree(rot.x), bs::Degree(rot.y), bs::Degree(rot.z));
}

bs::RTTITypeBase *CRotor::getRTTIStatic() { return CRotorRTTI::instance(); }

bs::RTTITypeBase *CRotor::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
