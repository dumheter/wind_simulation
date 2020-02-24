#include "moveable_state.hpp"

namespace wind {

MoveableState::MoveableState(UniqueId id)
    : m_id(id), m_type(ComponentTypes::kInvalid), m_flag(0),
      m_position(bs::Vector3::ONE), m_vel(), m_angVel(),
      m_rotation(bs::Quaternion::IDENTITY) {}

void MoveableState::from(const bs::Transform &transform) {
  m_position = transform.getPosition();
  m_rotation = transform.getRotation();
}

void MoveableState::from(bs::HSceneObject so) {
  from(so->getTransform());
  auto rigid = so->getComponent<bs::CRigidbody>();
  if (rigid) {
    from(rigid);
  }
}

void MoveableState::from(const MoveableState &other) {
  m_position = other.m_position;
  m_rotation = other.m_rotation;
}
void MoveableState::from(bs::HRigidbody rigid) {
  m_vel = rigid->getVelocity();
  m_angVel = rigid->getAngularVelocity();
}

void MoveableState::to(bs::Transform &transform) const {
  transform.setPosition(m_position);
  transform.setRotation(m_rotation);
}

void MoveableState::to(bs::HSceneObject &so) const {
  so->setPosition(m_position);
  so->setRotation(m_rotation);
}

bool MoveableState::ToBytes(alflib::RawMemoryWriter &mw) const {
  mw.Write(m_id);
  mw.Write(static_cast<u32>(m_type));
  mw.Write(m_position.x);
  mw.Write(m_position.y);
  mw.Write(m_position.z);
  mw.Write(m_rotation.x);
  mw.Write(m_rotation.y);
  mw.Write(m_rotation.z);
  mw.Write(m_rotation.w);
  auto res = mw.Write(m_flag);
  if (getRigid()) {
    mw.Write(m_vel.x);
    mw.Write(m_vel.y);
    mw.Write(m_vel.z);
    mw.Write(m_angVel.x);
    mw.Write(m_angVel.y);
    res = mw.Write(m_angVel.z);
  }
  return res;
}

MoveableState MoveableState::FromBytes(alflib::RawMemoryReader &mr) {
  MoveableState ms{mr.Read<decltype(ms.m_id)>()};
  ms.m_type = static_cast<ComponentTypes>(mr.Read<u32>());
  ms.m_position.x = mr.Read<decltype(ms.m_position.x)>();
  ms.m_position.y = mr.Read<decltype(ms.m_position.y)>();
  ms.m_position.z = mr.Read<decltype(ms.m_position.z)>();
  ms.m_rotation.x = mr.Read<decltype(ms.m_rotation.x)>();
  ms.m_rotation.y = mr.Read<decltype(ms.m_rotation.y)>();
  ms.m_rotation.z = mr.Read<decltype(ms.m_rotation.z)>();
  ms.m_rotation.w = mr.Read<decltype(ms.m_rotation.w)>();
  ms.m_flag = mr.Read<decltype(ms.m_flag)>();
  if (ms.getRigid()) {
    ms.m_vel.x = mr.Read<decltype(ms.m_vel.x)>();
    ms.m_vel.y = mr.Read<decltype(ms.m_vel.y)>();
    ms.m_vel.z = mr.Read<decltype(ms.m_vel.z)>();
    ms.m_angVel.x = mr.Read<decltype(ms.m_angVel.x)>();
    ms.m_angVel.y = mr.Read<decltype(ms.m_angVel.y)>();
    ms.m_angVel.z = mr.Read<decltype(ms.m_angVel.z)>();
  }
  return ms;
}

} // namespace wind
