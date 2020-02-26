#include "crotor.hpp"
#include "scene/wind_src.hpp"
#include "wind_src.hpp"

namespace wind {

CRotor::CRotor() : m_rotation() {}

CRotor::CRotor(const bs::HSceneObject &parent, const HCWindSource &windSource)
    : bs::Component(parent), m_rotation(), m_windSource(windSource) {
  setName("CRotor");
}

void CRotor::fixedUpdate() { SO()->yaw(bs::Degree(m_rotation)); }

void CRotor::setRotation(f32 degree) { m_rotation = degree; }

bs::RTTITypeBase *CRotor::getRTTIStatic() { return CRotorRTTI::instance(); }

bs::RTTITypeBase *CRotor::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
