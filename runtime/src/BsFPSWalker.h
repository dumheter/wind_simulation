#pragma once

#include "BsPrerequisites.h"
#include "Input/BsVirtualInput.h"
#include "Scene/BsComponent.h"

namespace bs {

class FPSWalker : public Component {
public:
  FPSWalker(const HSceneObject &parent);

  void update() override;

  void toggleGravity() { m_gravity = !m_gravity; }

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
};

using HFPSWalker = GameObjectHandle<FPSWalker>;
} // namespace bs
