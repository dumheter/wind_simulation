#include "moveable_state.hpp"

namespace wind {

MoveableState::MoveableState(UniqueId id)
    : id(id), flag(0), position(bs::Vector3::ONE), vel(), angVel(),
      rotation(bs::Quaternion::IDENTITY) {}

void MoveableState::from(const bs::Transform &transform) {
  position = transform.getPosition();
  rotation = transform.getRotation();
}

void MoveableState::from(bs::HSceneObject so) {
  from(so->getTransform());
  auto rigid = so->getComponent<bs::CRigidbody>();
  if (rigid) {
    from(rigid);
  }
}

void MoveableState::from(const MoveableState &other) {
  position = other.position;
  rotation = other.rotation;
}
void MoveableState::from(bs::HRigidbody rigid) {
  vel = rigid->getVelocity();
  angVel = rigid->getAngularVelocity();
}

void MoveableState::to(bs::Transform &transform) const {
  transform.setPosition(position);
  transform.setRotation(rotation);
}

void MoveableState::to(bs::HSceneObject &so) const {
  so->setPosition(position);
  so->setRotation(rotation);
}

bool MoveableState::ToBytes(alflib::RawMemoryWriter &mw) const {
  mw.Write(id);
  mw.Write(position.x);
  mw.Write(position.y);
  mw.Write(position.z);
  mw.Write(rotation.x);
  mw.Write(rotation.y);
  mw.Write(rotation.z);
  mw.Write(rotation.w);
  auto res = mw.Write(flag);
  if (getRigid()) {
    mw.Write(vel.x);
    mw.Write(vel.y);
    mw.Write(vel.z);
    mw.Write(angVel.x);
    mw.Write(angVel.y);
    res = mw.Write(angVel.z);
  }
  return res;
}

MoveableState MoveableState::FromBytes(alflib::RawMemoryReader &mr) {
  MoveableState ms{mr.Read<decltype(ms.id)>()};
  ms.position.x = mr.Read<decltype(ms.position.x)>();
  ms.position.y = mr.Read<decltype(ms.position.y)>();
  ms.position.z = mr.Read<decltype(ms.position.z)>();
  ms.rotation.x = mr.Read<decltype(ms.rotation.x)>();
  ms.rotation.y = mr.Read<decltype(ms.rotation.y)>();
  ms.rotation.z = mr.Read<decltype(ms.rotation.z)>();
  ms.rotation.w = mr.Read<decltype(ms.rotation.w)>();
  ms.flag = mr.Read<decltype(ms.flag)>();
  if (ms.getRigid()) {
    ms.vel.x = mr.Read<decltype(ms.vel.x)>();
    ms.vel.y = mr.Read<decltype(ms.vel.y)>();
    ms.vel.z = mr.Read<decltype(ms.vel.z)>();
    ms.angVel.x = mr.Read<decltype(ms.angVel.x)>();
    ms.angVel.y = mr.Read<decltype(ms.angVel.y)>();
    ms.angVel.z = mr.Read<decltype(ms.angVel.z)>();
  }
  return ms;
}

} // namespace wind
