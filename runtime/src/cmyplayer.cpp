#include "cmyplayer.hpp"
#include "Scene/BsSceneObject.h"
#include "bsFPSWalker.h"
#include "cnet_component.hpp"
#include "world.hpp"
#include "common.hpp"

namespace wind {

CMyPlayer::CMyPlayer(bs::HSceneObject parent, World *world)
    : Component(parent), m_client(world), m_world(world) {
  setName("CMyPlayer");
  m_fpsWalker = SO()->addComponent<bs::FPSWalker>();
  m_fpsWalker->makeActive();
  m_lastRotation = SO()->getTransform().getRotation();
}

bool CMyPlayer::isConnected() const {
  return m_client.GetConnectionState() == ConnectionState::kConnected;
}

void CMyPlayer::connect(const char *address) { m_client.Connect(address); }

void CMyPlayer::disconnect() { m_client.CloseConnection(); }

void CMyPlayer::update() { m_client.Poll(); }

void CMyPlayer::fixedUpdate() {
  if (!m_world->serverIsActive() &&
      m_client.GetConnectionState() == ConnectionState::kConnected &&
      m_fpsWalker->hasNewInput()) {
    auto &packet = m_client.getPacket();
    packet.ClearPayload();
    packet.SetHeader(PacketHeaderTypes::kPlayerTick);
    auto mw = packet.GetMemoryWriter();
    mw->Write(m_fpsWalker->getPlayerInput());
    auto rotation = SO()->getTransform().getRotation();
    const u8 rotChanged = m_lastRotation != rotation;
    mw->Write(rotChanged);
    if (rotChanged) {
      mw->Write(rotation.x);
      mw->Write(rotation.y);
      mw->Write(rotation.z);
      mw->Write(rotation.w);
      m_lastRotation = rotation;
    }
    mw.Finalize();
    m_client.PacketSend(packet, SendStrategy::kUnreliableNoDelay);
  }
}

} // namespace wind
