#pragma once

#include "BsPrerequisites.h"
#include "Input/BsVirtualInput.h"
#include "Scene/BsComponent.h"
#include "player_input.hpp"

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
} // namespace bs
