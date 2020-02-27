#pragma once

#include "shared/scene/rtti.hpp"
#include "shared/state/player_input.hpp"

#include <BsCorePrerequisites.h>
#include <Input/BsInputConfiguration.h>
#include <Math/BsQuaternion.h>
#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>
#include <String/BsString.h>

namespace bs {
class IReflectable;
}

namespace wind {

class FPSWalker : public bs::Component {
  friend class FPSWalkerRTTI;

public:
  FPSWalker(const bs::HSceneObject &parent);

  void update() override;

  void makeActive() { m_isActive = true; }

  void toggleGravity() { m_gravity = !m_gravity; }

  bool hasNewInput() const { return m_hasNewInput; }

  void resetNewInputFlag() { m_hasNewInput = false; }

  PlayerInput getPlayerInput() {
    m_hasNewInput = false;
    return m_lastInput;
  }

  void setInput(PlayerInput input) { m_lastInput = input; }

  void applyRotation(const bs::Quaternion &rotation);

  static bs::RTTITypeBase *getRTTIStatic();

  bs::RTTITypeBase *getRTTI() const override;

  FPSWalker() = default; // serialization

private:
  PlayerInput getInput();

  void applyInput(PlayerInput input);

private:
  bs::HCharacterController mController;

  float mCurrentSpeed = 0.0f;

  bs::VirtualButton mMoveForward;
  bs::VirtualButton mMoveBack;
  bs::VirtualButton mMoveLeft;
  bs::VirtualButton mMoveRight;
  bs::VirtualButton mFastMove;
  bs::VirtualButton mSpace;
  bs::VirtualButton mGravity;

  bool m_gravity = true;
  PlayerInput m_lastInput{};
  bool m_hasNewInput;
  bool m_isActive = false;
};

using HFPSWalker = bs::GameObjectHandle<FPSWalker>;

class FPSWalkerRTTI
    : public bs::RTTIType<FPSWalker, bs::Component, FPSWalkerRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  // BS_RTTI_MEMBER_PLAIN_NAMED(m_rotation, m_rotation, 0)
  BS_END_RTTI_MEMBERS

public:
  const bs::String &getRTTIName() override {
    static bs::String name = "FPSWalker";
    return name;
  }
  bs::UINT32 getRTTIId() override { return wind::TID_FPSWalker; }
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<FPSWalker>();
  }
};

} // namespace bs
