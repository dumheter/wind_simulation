#pragma once

#include "BsPrerequisites.h"
#include "Input/BsVirtualInput.h"
#include "Scene/BsComponent.h"
#include "cnet_component.hpp"

namespace bs {

class FPSWalker : public Component {
public:
  FPSWalker(const HSceneObject &parent, wind::HCNetComponent netComp);

  void update() override;

  void toggleGravity() { m_gravity = !m_gravity; }

private:
  HCharacterController mController;
  wind::HCNetComponent m_netComp;

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
