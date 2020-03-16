#include "fps_camera.hpp"

namespace wind {

constexpr float ROTATION_SPEED = 3.0f;

constexpr bs::Degree PITCH_RANGE = bs::Degree(90.0f);

FPSCamera::FPSCamera(const bs::HSceneObject &parent) : bs::Component(parent) {
  setName("FPSCamera");

  mHorizontalAxis = bs::VirtualAxis("Horizontal");
  mVerticalAxis = bs::VirtualAxis("Vertical");
  mUp = bs::VirtualButton("AUp");
  mDown = bs::VirtualButton("ADown");
  mLeft = bs::VirtualButton("ALeft");
  mRight = bs::VirtualButton("ARight");

  bs::Quaternion rotation = SO()->getTransform().getRotation();

  bs::Radian pitch, yaw, roll;
  (void)rotation.toEulerAngles(pitch, yaw, roll);

  mPitch = pitch;
  mYaw = yaw;

  applyAngles();
}

void FPSCamera::update() {
  if (m_enabled) {
    mYaw += bs::Degree(bs::gVirtualInput().getAxisValue(mHorizontalAxis) *
                       ROTATION_SPEED);
    mPitch += bs::Degree(bs::gVirtualInput().getAxisValue(mVerticalAxis) *
                         ROTATION_SPEED);
  }

  constexpr float kArrowRotSpeed = 1.5f;
  mPitch += bs::Degree(bs::gVirtualInput().isButtonHeld(mUp) * kArrowRotSpeed);
  mPitch -=
      bs::Degree(bs::gVirtualInput().isButtonHeld(mDown) * kArrowRotSpeed);
  mYaw += bs::Degree(bs::gVirtualInput().isButtonHeld(mLeft) * kArrowRotSpeed);
  mYaw -= bs::Degree(bs::gVirtualInput().isButtonHeld(mRight) * kArrowRotSpeed);

  applyAngles();
}

void FPSCamera::applyAngles() {
  mYaw.wrap();
  mPitch.wrap();

  const bs::Degree pitchMax = PITCH_RANGE;
  const bs::Degree pitchMin = bs::Degree(360.0f) - PITCH_RANGE;

  if (mPitch > pitchMax && mPitch < pitchMin) {
    if ((mPitch - pitchMax) > (pitchMin - mPitch))
      mPitch = pitchMin;
    else
      mPitch = pitchMax;
  }

  bs::Quaternion yRot(bs::Vector3::UNIT_Y, bs::Radian(mYaw));
  bs::Quaternion xRot(bs::Vector3::UNIT_X, bs::Radian(mPitch));

  if (!mCharacterSO) {
    bs::Quaternion camRot = yRot * xRot;
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

} // namespace wind
