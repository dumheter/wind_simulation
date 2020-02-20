#include "cplayer.hpp"
#include "BsApplication.h"
#include "Components/BsCCharacterController.h"
#include "Scene/BsSceneObject.h"

namespace wind {

CPlayer::CPlayer(const bs::HSceneObject &parent) : Component(parent) {
  m_controller = SO()->getComponent<bs::CCharacterController>();
}

} // namespace wind
