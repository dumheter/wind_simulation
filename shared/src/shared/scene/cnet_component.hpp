#pragma once

#include <BsPrerequisites.h>
#include <Private/RTTI/BsGameObjectRTTI.h>
#include <RTTI/BsMathRTTI.h>
#include <Reflection/BsRTTIPlain.h>
#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>

#include "shared/scene/rtti.hpp"
#include "shared/state/moveable_state.hpp"
#include "shared/types.hpp"
#include "shared/utility/unique_id.hpp"

namespace wind {

class World;

class CNetComponent : public bs::Component {
public:
  CNetComponent(); // serialization

  /**
   * Don't forget to add this component to the world.
   * world->addNetComp(...)
   */
  explicit CNetComponent(bs::HSceneObject parent);

  CNetComponent(bs::HSceneObject parent, const MoveableState &moveableState);

  void onCreated() override;

  void SODestroy() { SO()->destroy(); }

  void onTransformChanged(bs::TransformChangedFlags flags) override;

  /**
   * Must have a rigidbody component on the parent.
   */
  void addForce(bs::Vector3 force, bs::ForceMode mode);

  UniqueId getUniqueId() const { return m_state.id; }

  void setUniqueId(UniqueId id) { m_state.id = id; }

  const MoveableState &getState() const { return m_state; }

  /**
   * Ignores both id and type.
   */
  void setState(const MoveableState &moveableState);

  void setPosition(bs::Vector3 position);

  bool hasChanged() const { return m_hasChanged; }

  void resetChanged() { m_hasChanged = false; }

  bool operator==(const CNetComponent &other) const {
    return getUniqueId() == other.getUniqueId();
  }

  bool operator!=(const CNetComponent &other) const {
    return !(*this == other);
  }

  friend class CNetComponentRTTI;

  static bs::RTTITypeBase *getRTTIStatic();
  bs::RTTITypeBase *getRTTI() const override;

private:
  bool m_hasChanged;
  MoveableState m_state;
}; // namespace wind

using HCNetComponent = bs::GameObjectHandle<CNetComponent>;

class CNetComponentRTTI
    : public bs::RTTIType<CNetComponent, bs::Component, CNetComponentRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_RTTI_MEMBER_PLAIN(m_hasChanged, 0)
  BS_RTTI_MEMBER_PLAIN_NAMED(id, m_state.id, 1)
  BS_RTTI_MEMBER_PLAIN_NAMED(flag, m_state.flag, 2)
  BS_RTTI_MEMBER_PLAIN_NAMED(position, m_state.position, 3)
  BS_RTTI_MEMBER_PLAIN_NAMED(vel, m_state.vel, 4)
  BS_RTTI_MEMBER_PLAIN_NAMED(angVel, m_state.angVel, 5)
  BS_RTTI_MEMBER_PLAIN_NAMED(rotation, m_state.rotation, 6)
  BS_END_RTTI_MEMBERS

public:
  const bs::String &getRTTIName() override {
    static bs::String name = "CNetComponent";
    return name;
  }
  bs::UINT32 getRTTIId() override { return TID_CNetComponent; }
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CNetComponent>();
  }
};

} // namespace wind
