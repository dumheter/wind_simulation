#include "camera_flyer.h"

#include <BsApplication.h>
#include <Components/BsCCamera.h>
#include <Input/BsInput.h>
#include <Math/BsMath.h>
#include <Math/BsVector3.h>
#include <Platform/BsCursor.h>
#include <Scene/BsSceneObject.h>
#include <Utility/BsTime.h>

namespace bs {

const float CameraFlyer::START_SPEED = 8.0f;
const float CameraFlyer::TOP_SPEED = 24.0f;
const float CameraFlyer::ACCELERATION = 5.0f;
const float CameraFlyer::FAST_MODE_MULTIPLIER = 2.0f;
const float CameraFlyer::ROTATION_SPEED = 3.0f;

/** Wraps an angle so it always stays in [0, 360) range. */
Degree wrapAngle(Degree angle) {
  if (angle.valueDegrees() < -360.0f)
    angle += Degree(360.0f);

  if (angle.valueDegrees() > 360.0f)
    angle -= Degree(360.0f);

  return angle;
}

CameraFlyer::CameraFlyer(const HSceneObject &parent) : Component(parent) {
  // Set a name for the component, so we can find it later if needed
  setName("CameraFlyer");

  // Get handles for key bindings. Actual keys attached to these bindings will
  // be registered during app start-up.
  mMoveForward = VirtualButton("Forward");
  mMoveBack = VirtualButton("Back");
  mMoveLeft = VirtualButton("Left");
  mMoveRight = VirtualButton("Right");
  mMoveUp = VirtualButton("Up");
  mMoveDown = VirtualButton("Down");
  mFastMove = VirtualButton("FastMove");
  mRotateCam = VirtualButton("RotateCam");
  mHorizontalAxis = VirtualAxis("Horizontal");
  mVerticalAxis = VirtualAxis("Vertical");
}

void CameraFlyer::update() {
  // Check if any movement or rotation keys are being held
  const bool goingForward = gVirtualInput().isButtonHeld(mMoveForward);
  const bool goingBack = gVirtualInput().isButtonHeld(mMoveBack);
  const bool goingLeft = gVirtualInput().isButtonHeld(mMoveLeft);
  const bool goingRight = gVirtualInput().isButtonHeld(mMoveRight);
  const bool goingUp = gVirtualInput().isButtonHeld(mMoveUp);
  const bool goingDown = gVirtualInput().isButtonHeld(mMoveDown);
  const bool fastMove = gVirtualInput().isButtonHeld(mFastMove);
  const bool camRotating = gVirtualInput().isButtonHeld(mRotateCam);

  // If switch to or from rotation mode, hide or show the cursor
  if (camRotating != mLastButtonState) {
    if (camRotating) {
      Cursor::instance().hide();
    } else {
      Cursor::instance().show();
      const SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
      const UINT32 x =
          window->getProperties().left + (window->getProperties().width / 2);
      const UINT32 y =
          window->getProperties().top + (window->getProperties().height / 2);
      Cursor::instance().setScreenPosition(Vector2I(x, y));
    }

    mLastButtonState = camRotating;
  }

  // If camera is rotating, apply new pitch/yaw rotation values depending on the
  // amount of rotation from the vertical/horizontal axes.
  float frameDelta = gTime().getFrameDelta();
  if (camRotating) {
    mYaw +=
        Degree(gVirtualInput().getAxisValue(mHorizontalAxis) * ROTATION_SPEED);
    mPitch +=
        Degree(gVirtualInput().getAxisValue(mVerticalAxis) * ROTATION_SPEED);

    mYaw = wrapAngle(mYaw);
    mPitch = wrapAngle(mPitch);

    Quaternion yRot;
    yRot.fromAxisAngle(Vector3::UNIT_Y, Radian(mYaw));

    Quaternion xRot;
    xRot.fromAxisAngle(Vector3::UNIT_X, Radian(mPitch));

    Quaternion camRot = yRot * xRot;
    camRot.normalize();

    SO()->setRotation(camRot);
  }

  const Transform &tfrm = SO()->getTransform();

  // If the movement button is pressed, determine direction to move in
  Vector3 direction = Vector3::ZERO;
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
      multiplier = FAST_MODE_MULTIPLIER;

    // Calculate current speed of the camera
    mCurrentSpeed = Math::clamp(mCurrentSpeed + ACCELERATION * frameDelta,
                                START_SPEED, TOP_SPEED);
    mCurrentSpeed *= multiplier;
  } else {
    mCurrentSpeed = 0.0f;
  }

  // If the current speed isn't too small, move the camera in the wanted
  // direction
  float tooSmall = std::numeric_limits<float>::epsilon();
  if (mCurrentSpeed > tooSmall) {
    Vector3 velocity = direction * mCurrentSpeed;
    SO()->move(velocity * frameDelta);
  }
}
} // namespace bs