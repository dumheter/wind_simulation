#include "moveable_state.hpp"

namespace wind {

void MoveableState::from(const bs::Transform &transform) {
  m_position = transform.getPosition();
  m_rotation = transform.getRotation();
  m_scale = transform.getScale();
}

void MoveableState::from(bs::HSceneObject &so) { from(so->getTransform()); }

void MoveableState::from(const MoveableState &other) {
  m_position = other.m_position;
  m_rotation = other.m_rotation;
  m_scale = other.m_scale;
}

void MoveableState::to(bs::Transform &transform) const {
  transform.setPosition(m_position);
  transform.setRotation(m_rotation);
  transform.setScale(m_scale);
}

void MoveableState::to(bs::HSceneObject &so) const {
  so->setPosition(m_position);
  so->setRotation(m_rotation);
  so->setScale(m_scale);
}

bool MoveableState::ToBytes(alflib::RawMemoryWriter &mw) const {
  mw.Write(m_id);
  mw.Write(static_cast<u32>(m_type));
  mw.Write(m_position.x);
  mw.Write(m_position.y);
  mw.Write(m_position.z);
  mw.Write(m_scale.x);
  mw.Write(m_scale.y);
  mw.Write(m_scale.z);
  mw.Write(m_rotation.x);
  mw.Write(m_rotation.y);
  mw.Write(m_rotation.z);
  return mw.Write(m_rotation.w);
}

MoveableState MoveableState::FromBytes(alflib::RawMemoryReader &mr) {
  MoveableState ms{mr.Read<decltype(ms.m_id)>()};
  ms.m_type = static_cast<Creator::Types>(mr.Read<u32>());
  ms.m_position.x = mr.Read<decltype(ms.m_position.x)>();
  ms.m_position.y = mr.Read<decltype(ms.m_position.y)>();
  ms.m_position.z = mr.Read<decltype(ms.m_position.z)>();
  ms.m_scale.x = mr.Read<decltype(ms.m_scale.x)>();
  ms.m_scale.y = mr.Read<decltype(ms.m_scale.y)>();
  ms.m_scale.z = mr.Read<decltype(ms.m_scale.z)>();
  ms.m_rotation.x = mr.Read<decltype(ms.m_rotation.x)>();
  ms.m_rotation.y = mr.Read<decltype(ms.m_rotation.y)>();
  ms.m_rotation.z = mr.Read<decltype(ms.m_rotation.z)>();
  ms.m_rotation.w = mr.Read<decltype(ms.m_rotation.w)>();
  return ms;
}

} // namespace wind
