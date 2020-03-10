#pragma once

#include <BsPrerequisites.h>
#include <Math/BsQuaternion.h>
#include <Private/RTTI/BsGameObjectRTTI.h>
#include <RTTI/BsMathRTTI.h>
#include <Reflection/BsRTTIPlain.h>
#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>

#include "shared/scene/component_handles.hpp"
#include "shared/scene/rtti.hpp"
#include "shared/types.hpp"
#include "shared/math/math.hpp"

namespace wind {
class CRotor : public bs::Component {
public:
  friend class CRotorRTTI;

  CRotor(); // serialization

  CRotor(const bs::HSceneObject &parent, Vec3F rotation);

  void fixedUpdate() override;

  /**
   * Every fixed update, this rotation will be applied.
   */
  void setRotation(Vec3F rotation);

  static bs::RTTITypeBase *getRTTIStatic();
  bs::RTTITypeBase *getRTTI() const override;

private:
  bs::Quaternion m_rotation;
};

using HCRotor = bs::GameObjectHandle<CRotor>;

class CRotorRTTI : public bs::RTTIType<CRotor, bs::Component, CRotorRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_RTTI_MEMBER_PLAIN_NAMED(m_rotation, m_rotation, 0)
  BS_END_RTTI_MEMBERS

public:
  const bs::String &getRTTIName() override {
    static bs::String name = "CRotor";
    return name;
  }
  bs::UINT32 getRTTIId() override { return TID_CRotor; }
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CRotor>();
  }
};

} // namespace wind
