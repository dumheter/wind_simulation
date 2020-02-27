#pragma once

#include "shared/scene/rtti.hpp"

#include <BsPrerequisites.h>
#include <Input/BsVirtualInput.h>
#include <Math/BsDegree.h>
#include <Private/RTTI/BsGameObjectRTTI.h>
#include <Reflection/BsRTTIPlain.h>
#include <Reflection/BsRTTIType.h>

namespace bs {
class FPSCamera : public Component {
public:
  explicit FPSCamera(const HSceneObject &parent);

  void setCharacter(const HSceneObject &characterSO) {
    mCharacterSO = characterSO;
  }

  void setEnabled(bool enabled) { m_enabled = enabled; }

  void update() override;

  bs::HSceneObject getCamera() const { return SO(); }

  friend class FPSCameraRTTI;

  static bs::RTTITypeBase *getRTTIStatic();

  bs::RTTITypeBase *getRTTI() const override;

  FPSCamera() = default; // serialization

private:
  void applyAngles();

private:
  VirtualButton mUp;
  VirtualButton mDown;
  VirtualButton mLeft;
  VirtualButton mRight;
  HSceneObject mCharacterSO;
  Degree mPitch = Degree(0.0f);
  Degree mYaw = Degree(0.0f);
  VirtualAxis mVerticalAxis;
  VirtualAxis mHorizontalAxis;
  bool m_enabled = true;
};

using HFPSCamera = GameObjectHandle<FPSCamera>;

class FPSCameraRTTI
    : public bs::RTTIType<FPSCamera, bs::Component, FPSCameraRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_RTTI_MEMBER_PLAIN(m_enabled, 0)
  BS_END_RTTI_MEMBERS

public:
  const bs::String &getRTTIName() override {
    static bs::String name = "FPSCamera";
    return name;
  }
  bs::UINT32 getRTTIId() override { return wind::TID_FPSCamera; }
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<FPSCamera>();
  }
};

} // namespace bs
