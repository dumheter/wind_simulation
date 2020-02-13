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
#include "log.hpp"

namespace bs {
constexpr float START_SPEED = 4.0f; // m/s
constexpr float TOP_SPEED = 7.0f;   // m/s
constexpr float ACCELERATION = 1.5f;
constexpr float FAST_MODE_MULTIPLIER = 2.0f;

FPSWalker::FPSWalker(const HSceneObject &parent) : Component(parent) {
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

wind::PlayerInput FPSWalker::getInput() {
  wind::PlayerInput input{};
  input.inputs.forward = gVirtualInput().isButtonHeld(mMoveForward);
  input.inputs.back = gVirtualInput().isButtonHeld(mMoveBack) ? 1 : 0;
  input.inputs.left = gVirtualInput().isButtonHeld(mMoveLeft);
  input.inputs.right = gVirtualInput().isButtonHeld(mMoveRight);
  input.inputs.fast = gVirtualInput().isButtonHeld(mFastMove);
  input.inputs.jump = gVirtualInput().isButtonHeld(mSpace);
  input.inputs.gravity = gVirtualInput().isButtonDown(mGravity);
  return input;
}

void FPSWalker::update() {
  const wind::PlayerInput input = m_isActive ? getInput() : m_lastInput;
  applyInput(input);

  if (m_isActive && input != m_lastInput) {
    m_hasNewInput = true;
    m_lastInput = input;
  }
}

void FPSWalker::applyRotation(const bs::Quaternion &rotation)
{
  SO()->setRotation(rotation);
}

void FPSWalker::applyInput(wind::PlayerInput input) {
  const Transform &tfrm = SO()->getTransform();

  Vector3 direction = Vector3::ZERO;
  if (input.inputs.forward)
    direction += tfrm.getForward();
  if (input.inputs.back)
    direction -= tfrm.getForward();
  if (input.inputs.right)
    direction += tfrm.getRight();
  if (input.inputs.left)
    direction -= tfrm.getRight();

  // Eliminate vertical movement
  direction.y = 0.0f;
  direction.normalize();

  const float frameDelta = gTime().getFrameDelta();

  // If a direction is chosen, normalize it to determine final direction.
  if (direction.squaredLength() != 0) {
    direction.normalize();

    float multiplier = 1.0f;
    if (input.inputs.fast)
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

  if (input.inputs.gravity) {
    toggleGravity();
  }

  const Vector3 gravity = m_gravity ? SceneManager::instance()
                                          .getMainScene()
                                          ->getPhysicsScene()
                                          ->getGravity()
                                    : Vector3();

  const Vector3 jump =
      !input.inputs.jump ? Vector3() : Vector3(0.0f, 20.0f, 0.0f);

  mController->move((velocity + gravity + jump) * frameDelta);

}
} // namespace bs
