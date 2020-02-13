#ifndef CNET_COMPONENT_HPP_
#define CNET_COMPONENT_HPP_

#include "BsPrerequisites.h"
#include "Scene/BsComponent.h"
#include "Scene/BsGameObjectHandle.h"
#include "Scene/BsSceneObject.h"
#include "common.hpp"
#include "moveable_state.hpp"
#include "utility/unique_id.hpp"
#include "creator.hpp"

namespace wind {

class World;

class CNetComponent : public bs::Component {
public:
  /**
   * Dont forget to add this component to the world.
   * world->addNetComp(...)
   */
  CNetComponent(bs::HSceneObject parent);

  CNetComponent(bs::HSceneObject parent, const MoveableState &moveableState);

  void onCreated() override;

  void onTransformChanged(bs::TransformChangedFlags flags) override;

  UniqueId getUniqueId() const { return m_state.getUniqueId(); }

  const MoveableState &getState() const { return m_state; }

  /**
   * Ignores both id and type.
   */
  void setState(const MoveableState &moveableState);

  void setUniqueId(UniqueId id) { m_state.setUniqueId(id); }

  void setType(Creator::Types type) { m_state.setType(type); }

  void setPosition(bs::Vector3 position);

  bool operator==(const CNetComponent &other) const {
    return getUniqueId() == other.getUniqueId();
  }

  bool operator!=(const CNetComponent &other) const {
    return !(*this == other);
  }

private:
  MoveableState m_state;
};

using HCNetComponent = bs::GameObjectHandle<CNetComponent>;

} // namespace wind

#endif // CNET_COMPONENT_HPP_
