#include "cmyplayer.hpp"
#include "Scene/BsSceneObject.h"
#include "cnet_component.hpp"

namespace wind {

CMyPlayer::CMyPlayer(const bs::HSceneObject &parent, World *world)
    : Component(parent), m_client(world) {
  setName("CMyPlayer");
  auto netcomp = SO()->addComponent<CNetComponent>();
}

void CMyPlayer::update() {}

} // namespace wind
