#pragma once

#include "shared/state/player_input.hpp"
#include "shared/utility/rtti_types.hpp"

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
class World;
}

namespace bs {

class FPSWalker : public Component {
public:
  FPSWalker(const HSceneObject &parent, wind::World *world);

  void update() override;

  void makeActive() { m_isActive = true; }

  void toggleGravity() { m_gravity = !m_gravity; }

  bool hasNewInput() const { return m_hasNewInput; }

  void resetNewInputFlag() { m_hasNewInput = false; }

  wind::PlayerInput getPlayerInput() {
    m_hasNewInput = false;
    return m_lastInput;
  }

  void setInput(wind::PlayerInput input) { m_lastInput = input; }

  void applyRotation(const bs::Quaternion &rotation);

  friend class FPSWalkerRTTI;

  static bs::RTTITypeBase *getRTTIStatic();

  bs::RTTITypeBase *getRTTI() const override;

  FPSWalker() = default; // serialization

private:
  wind::PlayerInput getInput();

  void applyInput(wind::PlayerInput input);

private:
  HCharacterController mController;
  wind::World *m_world;

  float mCurrentSpeed = 0.0f;

  VirtualButton mMoveForward;
  VirtualButton mMoveBack;
  VirtualButton mMoveLeft;
  VirtualButton mMoveRight;
  VirtualButton mFastMove;
  VirtualButton mSpace;
  VirtualButton mGravity;

  bool m_gravity = true;
  wind::PlayerInput m_lastInput{};
  bool m_hasNewInput;
  bool m_isActive = false;
};

using HFPSWalker = GameObjectHandle<FPSWalker>;

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
