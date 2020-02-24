#include "BsFPSCamera.h"

namespace bs {

constexpr float ROTATION_SPEED = 3.0f;

constexpr Degree PITCH_RANGE = Degree(30.0f);

FPSCamera::FPSCamera(const HSceneObject &parent) : Component(parent) {
  setName("FPSCamera");

  mHorizontalAxis = VirtualAxis("Horizontal");
  mVerticalAxis = VirtualAxis("Vertical");
  mUp = VirtualButton("AUp");
  mDown = VirtualButton("ADown");
  mLeft = VirtualButton("ALeft");
  mRight = VirtualButton("ARight");

  Quaternion rotation = SO()->getTransform().getRotation();

  Radian pitch, yaw, roll;
  (void)rotation.toEulerAngles(pitch, yaw, roll);

  mPitch = pitch;
  mYaw = yaw;

  applyAngles();
}

void FPSCamera::update() {
  if (m_enabled) {
    mYaw +=
        Degree(gVirtualInput().getAxisValue(mHorizontalAxis) * ROTATION_SPEED);
    mPitch +=
        Degree(gVirtualInput().getAxisValue(mVerticalAxis) * ROTATION_SPEED);
  }

  constexpr float kArrowRotSpeed = 1.5f;
  mPitch += Degree(gVirtualInput().isButtonHeld(mUp) * kArrowRotSpeed);
  mPitch -= Degree(gVirtualInput().isButtonHeld(mDown) * kArrowRotSpeed);
  mYaw += Degree(gVirtualInput().isButtonHeld(mLeft) * kArrowRotSpeed);
  mYaw -= Degree(gVirtualInput().isButtonHeld(mRight) * kArrowRotSpeed);

  applyAngles();
}

void FPSCamera::applyAngles() {
  mYaw.wrap();
  mPitch.wrap();

  const Degree pitchMax = PITCH_RANGE;
  const Degree pitchMin = Degree(360.0f) - PITCH_RANGE;

  if (mPitch > pitchMax && mPitch < pitchMin) {
    if ((mPitch - pitchMax) > (pitchMin - mPitch))
      mPitch = pitchMin;
    else
      mPitch = pitchMax;
  }

  Quaternion yRot(Vector3::UNIT_Y, Radian(mYaw));
  Quaternion xRot(Vector3::UNIT_X, Radian(mPitch));

  if (!mCharacterSO) {
    Quaternion camRot = yRot * xRot;
    camRot.normalize();

    SO()->setRotation(camRot);
  } else {
    mCharacterSO->setRotation(yRot);
    SO()->setRotation(xRot);
  }
}

bs::RTTITypeBase *FPSCamera::getRTTIStatic() {
  return FPSCameraRTTI::instance();
}

bs::RTTITypeBase *FPSCamera::getRTTI() const { return getRTTIStatic(); }

} // namespace bs
