#pragma once

#include "shared/scene/rtti.hpp"

#include <Input/BsVirtualInput.h>
#include <Math/BsDegree.h>
#include <Private/RTTI/BsGameObjectRTTI.h>
#include <Reflection/BsRTTIPlain.h>
#include <Reflection/BsRTTIType.h>

namespace wind {

class FPSCamera : public bs::Component {
  friend class FPSCameraRTTI;

public:
  explicit FPSCamera(const bs::HSceneObject &parent);

  void setCharacter(const bs::HSceneObject &characterSO) {
    mCharacterSO = characterSO;
  }

  void setEnabled(bool enabled) { m_enabled = enabled; }

  void update() override;

  bs::HSceneObject getCamera() const { return SO(); }

  static bs::RTTITypeBase *getRTTIStatic();

  bs::RTTITypeBase *getRTTI() const override;

  FPSCamera() = default; // serialization

private:
  void applyAngles();

private:
  bs::VirtualButton mUp;
  bs::VirtualButton mDown;
  bs::VirtualButton mLeft;
  bs::VirtualButton mRight;
  bs::HSceneObject mCharacterSO;
  bs::Degree mPitch = bs::Degree(0.0f);
  bs::Degree mYaw = bs::Degree(0.0f);
  bs::VirtualAxis mVerticalAxis;
  bs::VirtualAxis mHorizontalAxis;
  bool m_enabled = true;
};

using HFPSCamera = bs::GameObjectHandle<FPSCamera>;

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

} // namespace wind
