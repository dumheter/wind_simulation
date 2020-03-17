#include "fps_walker.hpp"
#include "Components/BsCCharacterController.h"
#include "Input/BsVirtualInput.h"
#include "Physics/BsPhysics.h"
#include "Scene/BsSceneManager.h"

namespace wind {
constexpr float START_SPEED = 4.0f; // m/s
constexpr float TOP_SPEED = 7.0f;   // m/s
constexpr float ACCELERATION = 1.5f;
constexpr float FAST_MODE_MULTIPLIER = 2.0f;

FPSWalker::FPSWalker(const bs::HSceneObject &parent) : bs::Component(parent) {
  setName("FPSWalker");
  mController = SO()->getComponent<bs::CCharacterController>();
  mMoveForward = bs::VirtualButton("Forward");
  mMoveBack = bs::VirtualButton("Back");
  mMoveLeft = bs::VirtualButton("Left");
  mMoveRight = bs::VirtualButton("Right");
  mFastMove = bs::VirtualButton("FastMove");
  mSpace = bs::VirtualButton("Space");
  mGravity = bs::VirtualButton("Gravity");
}

PlayerInput FPSWalker::getInput() {
  PlayerInput input{};
  input.inputs.forward = bs::gVirtualInput().isButtonHeld(mMoveForward);
  input.inputs.back = bs::gVirtualInput().isButtonHeld(mMoveBack) ? 1 : 0;
  input.inputs.left = bs::gVirtualInput().isButtonHeld(mMoveLeft);
  input.inputs.right = bs::gVirtualInput().isButtonHeld(mMoveRight);
  input.inputs.fast = bs::gVirtualInput().isButtonHeld(mFastMove);
  input.inputs.jump = bs::gVirtualInput().isButtonHeld(mSpace);
  input.inputs.gravity = bs::gVirtualInput().isButtonDown(mGravity);
  return input;
}

void FPSWalker::update() {
  const PlayerInput input = m_isActive ? getInput() : m_lastInput;
  applyInput(input);

  if (m_isActive && input != m_lastInput) {
    m_hasNewInput = true;
    m_lastInput = input;
  }
}

void FPSWalker::applyRotation(const bs::Quaternion &rotation) {
  SO()->setRotation(rotation);
}

void FPSWalker::applyInput(PlayerInput input) {
  const bs::Transform &tfrm = SO()->getTransform();

  bs::Vector3 direction = bs::Vector3::ZERO;
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

  const float frameDelta = bs::gTime().getFrameDelta();

  // If a direction is chosen, normalize it to determine final direction.
  if (direction.squaredLength() != 0) {
    direction.normalize();

    float multiplier = 1.0f;
    if (input.inputs.fast)
      multiplier = FAST_MODE_MULTIPLIER;

    mCurrentSpeed = bs::Math::clamp(mCurrentSpeed + ACCELERATION * frameDelta,
                                    START_SPEED, TOP_SPEED);
    mCurrentSpeed *= multiplier;
  } else {
    mCurrentSpeed = 0.0f;
  }

  bs::Vector3 velocity(bs::BsZero);
  float tooSmall = std::numeric_limits<float>::epsilon();
  if (mCurrentSpeed > tooSmall)
    velocity = direction * mCurrentSpeed;

  if (input.inputs.gravity) {
    toggleGravity();
  }

  const bs::Vector3 gravity = m_gravity ? bs::SceneManager::instance()
                                              .getMainScene()
                                              ->getPhysicsScene()
                                              ->getGravity()
                                        : bs::Vector3();

  const bs::Vector3 jump =
      !input.inputs.jump ? bs::Vector3() : bs::Vector3(0.0f, 20.0f, 0.0f);

  mController->move((velocity + gravity + jump) * frameDelta);
}

bs::RTTITypeBase *FPSWalker::getRTTIStatic() {
  return FPSWalkerRTTI::instance();
}

bs::RTTITypeBase *FPSWalker::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
