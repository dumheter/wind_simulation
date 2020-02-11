#include "cnet_component.hpp"
#include "log.hpp"
#include "world.hpp"

namespace wind {

CNetComponent::CNetComponent(const bs::HSceneObject &parent)
    : bs::Component(parent), m_state(MoveableState::generateNew()) {
  setName("NetComp");
}

void CNetComponent::onTransformChanged(bs::TransformChangedFlags flags) {
  logVeryVerbose("transform changed");
  m_state.from(SO()->getTransform());
}

} // namespace wind
