#include "crotor.hpp"
#include "shared/scene/wind_src.hpp"

namespace wind {

CRotor::CRotor() : m_rotation() {}

CRotor::CRotor(const bs::HSceneObject &parent, const HCWindSource &windSource)
    : bs::Component(parent), m_rotation(), m_windSource(windSource) {
  setName("CRotor");
}

void CRotor::fixedUpdate() { SO()->rotate(m_rotation); }

void CRotor::setRotation(bs::Vector3 rot) {
  m_rotation =
      bs::Quaternion(bs::Degree(rot.x), bs::Degree(rot.y), bs::Degree(rot.z));
}

bs::RTTITypeBase *CRotor::getRTTIStatic() { return CRotorRTTI::instance(); }

bs::RTTITypeBase *CRotor::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
