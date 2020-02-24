#include "cnet_component.hpp"

namespace wind {

CNetComponent::CNetComponent() : m_hasChanged(false), m_state() {}

CNetComponent::CNetComponent(bs::HSceneObject parent)
    : bs::Component(parent), m_hasChanged(false),
      m_state(MoveableState::generateNew()) {
  mNotifyFlags = bs::TCF_Transform;
  setName("NetComp");
}

CNetComponent::CNetComponent(bs::HSceneObject parent,
                             const MoveableState &moveableState)
    : bs::Component(parent), m_hasChanged(false), m_state(moveableState) {
  setName("NetComp");
  mNotifyFlags = bs::TCF_Transform;
}

void CNetComponent::onCreated() {
  mNotifyFlags = (bs::TransformChangedFlags::TCF_None);
  m_hasChanged = true;
  SO()->setPosition(m_state.getPosition());
  SO()->setRotation(m_state.getRotation());
  auto rigid = SO()->getComponent<bs::CRigidbody>();
  if (rigid) {
    rigid->setVelocity(m_state.getVel());
    rigid->setAngularVelocity(m_state.getAngVel());
  }
  mNotifyFlags = bs::TCF_Transform;
}

void CNetComponent::onTransformChanged(bs::TransformChangedFlags flags) {
  m_hasChanged = true;
  m_state.from(SO());
}

void CNetComponent::addForce(bs::Vector3 force, bs::ForceMode mode) {
  auto rigid = SO()->getComponent<bs::CRigidbody>();
  if (rigid) {
    rigid->addForce(force, mode);
  }
}

void CNetComponent::setState(const MoveableState &moveableState) {
  mNotifyFlags = (bs::TransformChangedFlags::TCF_None);
  m_hasChanged = true;
  m_state = moveableState;
  SO()->setPosition(m_state.getPosition());
  SO()->setRotation(m_state.getRotation());
  auto rigid = SO()->getComponent<bs::CRigidbody>();
  if (rigid) {
    rigid->setVelocity(m_state.getVel());
    rigid->setAngularVelocity(m_state.getAngVel());
  }
  mNotifyFlags = bs::TCF_Transform;
}

void CNetComponent::setType(Creator::Types type) {
  m_state.setType(type);
  if (type != Creator::Types::kPlayer && type != Creator::Types::kInvalid) {
    m_state.setRigid(true);
  }
}

void CNetComponent::setPosition(bs::Vector3 position) {
  mNotifyFlags = (bs::TransformChangedFlags::TCF_None);
  m_hasChanged = true;
  m_state.setPosition(position);
  SO()->setPosition(m_state.getPosition());
  mNotifyFlags = bs::TCF_Transform;
}

bs::RTTITypeBase *CNetComponent::getRTTIStatic() {
  return CNetComponentRTTI::instance();
}

bs::RTTITypeBase *CNetComponent::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
