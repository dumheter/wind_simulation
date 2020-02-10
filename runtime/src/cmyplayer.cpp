#include "cmyplayer.hpp"

namespace wind {

CMyPlayer::CMyPlayer(const bs::HSceneObject &parent, World *world)
    : Component(parent), m_client(world) {
  setName("CMyPlayer");
}

void CMyPlayer::update() {}

} // namespace wind
