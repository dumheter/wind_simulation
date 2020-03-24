#include "camera_flyer.h"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/math/math.hpp"

#include <BsApplication.h>
#include <Components/BsCCamera.h>
#include <Input/BsInput.h>
#include <Math/BsMath.h>
#include <Math/BsVector3.h>
#include <Platform/BsCursor.h>
#include <Scene/BsSceneObject.h>
#include <Utility/BsTime.h>

// ========================================================================== //
// CameraFlyer Implementation
// ========================================================================== //

namespace wind {

/** Wraps an angle so it always stays in [0, 360) range. */
bs::Degree wrapAngle(bs::Degree angle) {
  if (angle.valueDegrees() < -360.0f)
    angle += bs::Degree(360.0f);

  if (angle.valueDegrees() > 360.0f)
    angle -= bs::Degree(360.0f);

  return angle;
}

CCameraFlyer::CCameraFlyer(const bs::HSceneObject &parent) : Component(parent) {
  // Set a name for the component, so we can find it later if needed
  setName("CameraFlyer");

  // Get handles for key bindings. Actual keys attached to these bindings will
  // be registered during app start-up.
  mMoveForward = bs::VirtualButton("Forward");
  mMoveBack = bs::VirtualButton("Back");
  mMoveLeft = bs::VirtualButton("Left");
  mMoveRight = bs::VirtualButton("Right");
  mMoveUp = bs::VirtualButton("Up");
  mMoveDown = bs::VirtualButton("Down");
  mFastMove = bs::VirtualButton("FastMove");
  mRotateCam = bs::VirtualButton("RotateCam");
  mPanCam = bs::VirtualButton("PanCam");
  mHorizontalAxis = bs::VirtualAxis("Horizontal");
  mVerticalAxis = bs::VirtualAxis("Vertical");
}

void CCameraFlyer::update() {
  // Check if any movement or rotation keys are being held
  const bool goingForward = bs::gVirtualInput().isButtonHeld(mMoveForward);
  const bool goingBack = bs::gVirtualInput().isButtonHeld(mMoveBack);
  const bool goingLeft = bs::gVirtualInput().isButtonHeld(mMoveLeft);
  const bool goingRight = bs::gVirtualInput().isButtonHeld(mMoveRight);
  const bool goingUp = bs::gVirtualInput().isButtonHeld(mMoveUp);
  const bool goingDown = bs::gVirtualInput().isButtonHeld(mMoveDown);
  const bool fastMove = bs::gVirtualInput().isButtonHeld(mFastMove);
  const bool camRotating = bs::gVirtualInput().isButtonHeld(mRotateCam);
  const bool camPanning = bs::gVirtualInput().isButtonHeld(mPanCam);

  // Enter/Exit camera mode
  if (camRotating != mWasInCameraMode || camPanning != mWasInCameraMode) {
    if (camRotating || camPanning) {
      bs::Cursor::instance().hide();
    } else {
      bs::Cursor::instance().show();
      const bs::SPtr<bs::RenderWindow> window =
          bs::gApplication().getPrimaryWindow();
      const u32 x =
          window->getProperties().left + (window->getProperties().width / 2);
      const u32 y =
          window->getProperties().top + (window->getProperties().height / 2);
      bs::Cursor::instance().setScreenPosition(Vec2I(x, y));
    }
    mWasInCameraMode = camRotating || camPanning;
  }

  // Handle camera rotation
  const float frameDelta = bs::gTime().getFrameDelta();
  if (camRotating) {
    mYaw +=
        bs::Degree(bs::gVirtualInput().getAxisValue(mHorizontalAxis) * kRotVel);
    mPitch +=
        bs::Degree(bs::gVirtualInput().getAxisValue(mVerticalAxis) * kRotVel);

    mYaw = wrapAngle(mYaw);
    mPitch = wrapAngle(mPitch);

    Quat yRot;
    yRot.fromAxisAngle(Vec3F::UNIT_Y, bs::Radian(mYaw));

    Quat xRot;
    xRot.fromAxisAngle(Vec3F::UNIT_X, bs::Radian(mPitch));

    Quat camRot = yRot * xRot;
    camRot.normalize();

    SO()->setRotation(camRot);
  }

  const bs::Transform &tfrm = SO()->getTransform();

  // Handle camera pan
  if (camPanning) {
    const f32 h =
        bs::gVirtualInput().getAxisValue(mHorizontalAxis) * kCamPanVel;
    const f32 v = bs::gVirtualInput().getAxisValue(mVerticalAxis) * kCamPanVel;
    SO()->move(tfrm.getRight() * -h + tfrm.getUp() * v);
  }

  // If the movement button is pressed, determine direction to move in
  Vec3F direction = Vec3F::ZERO;
  if (goingForward)
    direction += tfrm.getForward();
  if (goingBack)
    direction -= tfrm.getForward();
  if (goingRight)
    direction += tfrm.getRight();
  if (goingLeft)
    direction -= tfrm.getRight();

  if (goingUp)
    direction += tfrm.getUp();
  if (goingDown)
    direction -= tfrm.getUp();

  // If a direction is chosen, normalize it to determine final direction.
  if (direction.squaredLength() != 0) {
    direction.normalize();

    // Apply fast move multiplier if the fast move button is held.
    float multiplier = 1.0f;
    if (fastMove)
      multiplier = kFastMultiplier;

    // Calculate current speed of the camera
    mCurrentSpeed =
        clamp(mCurrentSpeed + kCamAcc * frameDelta, kCamVelStart, kCamVelTop);
    mCurrentSpeed *= multiplier;
  } else {
    mCurrentSpeed = 0.0f;
  }

  // If the current speed isn't too small, move the camera in the wanted
  // direction
  const f32 tooSmall = std::numeric_limits<float>::epsilon();
  if (mCurrentSpeed > tooSmall) {
    const Vec3F velocity = direction * mCurrentSpeed;
    SO()->move(velocity * frameDelta);
  }
}
} // namespace wind