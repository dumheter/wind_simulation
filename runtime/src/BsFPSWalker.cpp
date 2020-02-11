#include "BsFPSWalker.h"

#include <Scene/BsSceneManager.h>

#include "BsApplication.h"
#include "Components/BsCCamera.h"
#include "Components/BsCCharacterController.h"
#include "Math/BsMath.h"
#include "Math/BsQuaternion.h"
#include "Math/BsVector3.h"
#include "Physics/BsPhysics.h"
#include "Scene/BsSceneObject.h"
#include "Utility/BsTime.h"

namespace bs {
constexpr float START_SPEED = 4.0f; // m/s
constexpr float TOP_SPEED = 7.0f;   // m/s
constexpr float ACCELERATION = 1.5f;
constexpr float FAST_MODE_MULTIPLIER = 2.0f;

FPSWalker::FPSWalker(const HSceneObject &parent)
    : Component(parent) {
  setName("FPSWalker");
  mController = SO()->getComponent<CCharacterController>();
  mMoveForward = VirtualButton("Forward");
  mMoveBack = VirtualButton("Back");
  mMoveLeft = VirtualButton("Left");
  mMoveRight = VirtualButton("Right");
  mFastMove = VirtualButton("FastMove");
  mSpace = VirtualButton("Space");
  mGravity = VirtualButton("Gravity");
}

void FPSWalker::update() {
  bool goingForward = gVirtualInput().isButtonHeld(mMoveForward);
  bool goingBack = gVirtualInput().isButtonHeld(mMoveBack);
  bool goingLeft = gVirtualInput().isButtonHeld(mMoveLeft);
  bool goingRight = gVirtualInput().isButtonHeld(mMoveRight);
  bool fastMove = gVirtualInput().isButtonHeld(mFastMove);
  bool jumping = gVirtualInput().isButtonHeld(mSpace);

  const Transform &tfrm = SO()->getTransform();

  Vector3 direction = Vector3::ZERO;
  if (goingForward)
    direction += tfrm.getForward();
  if (goingBack)
    direction -= tfrm.getForward();
  if (goingRight)
    direction += tfrm.getRight();
  if (goingLeft)
    direction -= tfrm.getRight();

  // Eliminate vertical movement
  direction.y = 0.0f;
  direction.normalize();

  const float frameDelta = gTime().getFrameDelta();

  // If a direction is chosen, normalize it to determine final direction.
  if (direction.squaredLength() != 0) {
    direction.normalize();

    float multiplier = 1.0f;
    if (fastMove)
      multiplier = FAST_MODE_MULTIPLIER;

    mCurrentSpeed = Math::clamp(mCurrentSpeed + ACCELERATION * frameDelta,
                                START_SPEED, TOP_SPEED);
    mCurrentSpeed *= multiplier;
  } else {
    mCurrentSpeed = 0.0f;
  }

  Vector3 velocity(BsZero);
  float tooSmall = std::numeric_limits<float>::epsilon();
  if (mCurrentSpeed > tooSmall)
    velocity = direction * mCurrentSpeed;

  if (gVirtualInput().isButtonDown(mGravity)) {
    toggleGravity();
  }

  const Vector3 gravity = m_gravity ? SceneManager::instance()
                                          .getMainScene()
                                          ->getPhysicsScene()
                                          ->getGravity()
                                    : Vector3();

  const Vector3 jump = !jumping ? Vector3() : Vector3(0.0f, 20.0f, 0.0f);

  mController->move((velocity + gravity + jump) * frameDelta);
}
} // namespace bs
