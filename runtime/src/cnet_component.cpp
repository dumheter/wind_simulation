#include "cnet_component.hpp"
#include "Components/BsCRigidbody.h"
#include "Physics/BsRigidbody.h"
#include "log.hpp"
#include "world.hpp"

namespace wind {

CNetComponent::CNetComponent(bs::HSceneObject parent)
    : bs::Component(parent), m_state(MoveableState::generateNew()) {
  mNotifyFlags = bs::TCF_Transform;
  setName("NetComp");
}

CNetComponent::CNetComponent(bs::HSceneObject parent,
                             const MoveableState &moveableState)
    : bs::Component(parent), m_state(moveableState) {
  setName("NetComp");
  mNotifyFlags = bs::TCF_Transform;
}

void CNetComponent::onCreated() {
  mNotifyFlags = (bs::TransformChangedFlags::TCF_None);
  m_hasChanged = true;
  SO()->setPosition(m_state.getPosition());
  SO()->setScale(m_state.getScale());
  SO()->setRotation(m_state.getRotation());
  mNotifyFlags = bs::TCF_Transform;
}

void CNetComponent::onTransformChanged(bs::TransformChangedFlags flags) {
  m_hasChanged = true;
  m_state.from(SO()->getTransform());
}

void CNetComponent::setState(const MoveableState &moveableState) {
  mNotifyFlags = (bs::TransformChangedFlags::TCF_None);
  m_hasChanged = true;
  m_state = moveableState;
  SO()->setPosition(m_state.getPosition());
  SO()->setScale(m_state.getScale());
  SO()->setRotation(m_state.getRotation());
  mNotifyFlags = bs::TCF_Transform;
}

void CNetComponent::setPosition(bs::Vector3 position) {
  mNotifyFlags = (bs::TransformChangedFlags::TCF_None);
  m_hasChanged = true;
  m_state.setPosition(position);
  SO()->setPosition(m_state.getPosition());
  mNotifyFlags = bs::TCF_Transform;
}

} // namespace wind
