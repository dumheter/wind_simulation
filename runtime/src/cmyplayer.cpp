#include "cmyplayer.hpp"
#include "Scene/BsSceneObject.h"
#include "cnet_component.hpp"

namespace wind {

CMyPlayer::CMyPlayer(bs::HSceneObject parent, World *world)
    : Component(parent), m_client(world) {
  setName("CMyPlayer");
}

bool CMyPlayer::isConnected() const {
  return m_client.GetConnectionState() == ConnectionState::kConnected;
}

void CMyPlayer::connect(const char *address) { m_client.Connect(address); }

void CMyPlayer::disconnect() { m_client.CloseConnection(); }

void CMyPlayer::update() { m_client.Poll(); }

} // namespace wind
