#pragma once

#include "BsPrerequisites.h"
#include "Input/BsVirtualInput.h"
#include "Math/BsDegree.h"
#include "Scene/BsComponent.h"

namespace bs {
class FPSCamera : public Component {
public:
  FPSCamera(const HSceneObject &parent);

  void setCharacter(const HSceneObject &characterSO) {
    mCharacterSO = characterSO;
  }

  void setEnabled(bool enabled) { m_enabled = enabled; }

  void update() override;

  bs::HSceneObject getCamera() const { return SO(); }

private:
  void applyAngles();

  HSceneObject mCharacterSO;
  Degree mPitch = Degree(0.0f);
  Degree mYaw = Degree(0.0f);
  VirtualAxis mVerticalAxis;
  VirtualAxis mHorizontalAxis;
  bool m_enabled = true;
};

using HFPSCamera = GameObjectHandle<FPSCamera>;
} // namespace bs
