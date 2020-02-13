#pragma once

#include "BsPrerequisites.h"
#include "Input/BsVirtualInput.h"
#include "Scene/BsComponent.h"
#include "player_input.hpp"

namespace bs {

class FPSWalker : public Component {
public:
  FPSWalker(const HSceneObject &parent);

  void update() override;

  void makeActive() { m_isActive = true; }

  void toggleGravity() { m_gravity = !m_gravity; }

  bool hasNewInput() const { return m_hasNewInput; }

  wind::PlayerInput getPlayerInput() const { return m_lastInput; }

  void setInput(wind::PlayerInput input) { m_lastInput = input; }

  void applyRotation(const bs::Quaternion& rotation);

 private:
  wind::PlayerInput getInput();

  void applyInput(wind::PlayerInput input);

private:
  HCharacterController mController;

  float mCurrentSpeed = 0.0f;

  VirtualButton mMoveForward;
  VirtualButton mMoveBack;
  VirtualButton mMoveLeft;
  VirtualButton mMoveRight;
  VirtualButton mFastMove;
  VirtualButton mSpace;
  VirtualButton mGravity;

  bool m_gravity = true;
  wind::PlayerInput m_lastInput;
  bool m_hasNewInput;
  bool m_isActive = false;
};

using HFPSWalker = GameObjectHandle<FPSWalker>;
} // namespace bs
