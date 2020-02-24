#pragma once

#include "BsPrerequisites.h"
#include "Math/BsQuaternion.h"
#include "Private/RTTI/BsGameObjectRTTI.h"
#include "RTTI/BsMathRTTI.h"
#include "Reflection/BsRTTIPlain.h"
#include "Reflection/BsRTTIType.h"
#include "Scene/BsComponent.h"
#include "types.hpp"
#include "utility/rtti_types.hpp"

namespace wind {
class CRotor : public bs::Component {
public:
  CRotor(); // serialization

  explicit CRotor(const bs::HSceneObject &parent);

  void fixedUpdate() override;

  /**
   * Every fixed update, this rotation will be applied.
   */
  void setRotation(f32 degree);

  friend class CRotorRTTI;

  static bs::RTTITypeBase *getRTTIStatic();
  bs::RTTITypeBase *getRTTI() const override;

private:
  f32 m_rotation;
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
