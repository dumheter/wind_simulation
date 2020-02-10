#include "cnet_component.hpp"
#include "world.hpp"

namespace wind {

CNetComponent::CNetComponent(bs::HSceneObject parent)
    : bs::Component(parent), m_state(MoveableState::generateNew()) {}

void CNetComponent::onTransformChanged(bs::TransformChangedFlags flags) {
  m_state.from(SO()->getTransform());
}

} // namespace wind
