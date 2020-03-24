#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <shared/types.hpp>

#include "Input/BsVirtualInput.h"
#include "Math/BsMath.h"
#include "Scene/BsComponent.h"

// ========================================================================== //
// CameraFlyer Declaration
// ========================================================================== //

namespace wind {

/** Component that controls movement and rotation of the scene object it's
 * attached to. */
class CCameraFlyer : public bs::Component {
public:
  CCameraFlyer(const bs::HSceneObject &parent);

  /** Triggered once per frame. Allows the component to handle input and move.
   */
  void update() override;

private:
  float mCurrentSpeed; /**< Current speed of the camera. */

  bs::Degree mPitch = bs::Degree(0.0f); ///< Current camera pitch
  bs::Degree mYaw = bs::Degree(0.0f);   ///< Current camera yaw
  bool mWasInCameraMode = false;        ///< Whether camera was activated

  bs::VirtualButton mMoveForward;  ///< Camera forward.
  bs::VirtualButton mMoveBack;     ///< Camera backward.
  bs::VirtualButton mMoveLeft;     ///< Camera left.
  bs::VirtualButton mMoveRight;    ///< Camera right.
  bs::VirtualButton mMoveUp;       ///<
  bs::VirtualButton mMoveDown;     ///<
  bs::VirtualButton mFastMove;     ///< Key that speeds up movement while held.
  bs::VirtualButton mRotateCam;    ///< Right mouse rotate
  bs::VirtualButton mPanCam;       ///< Middle mouse panning
  bs::VirtualAxis mVerticalAxis;   ///< Camera pitch
  bs::VirtualAxis mHorizontalAxis; ///< Camera yaw

  static constexpr f32 kCamVelStart = 8.0f; ///< Initial cmaera velocity.
  static constexpr f32 kCamVelTop = 24.0f;  ///< Maximum cmaera velocity.
  static constexpr f32 kCamAcc = 5.0f;      ///< Camera acceleration

  static constexpr f32 kFastMultiplier = 2.0f; ///< Multiplier for fast speed
  static constexpr f32 kRotVel = 3.0f;         ///< Camera rotation velocity
  static constexpr f32 kCamPanVel = 1.0f;      ///< Camera pan velocity
};

using HCCameraFlyer = bs::GameObjectHandle<CCameraFlyer>;

} // namespace wind