#include "cnet_component.hpp"
#include "log.hpp"
#include "world.hpp"

namespace wind {

CNetComponent::CNetComponent(const bs::HSceneObject &parent)
    : bs::Component(parent), m_state(MoveableState::generateNew()) {
  mNotifyFlags = bs::TCF_Transform;
  setName("NetComp");
}

void CNetComponent::onTransformChanged(bs::TransformChangedFlags flags) {
  m_state.from(SO()->getTransform());
}

} // namespace wind
