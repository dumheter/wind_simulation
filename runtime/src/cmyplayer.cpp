#include "cmyplayer.hpp"
#include "world.hpp"

namespace wind {

CMyPlayer::CMyPlayer()
    : m_world(nullptr), m_fpsWalker(), m_lastRotation(), m_weapon(), m_fire1(),
      m_fire2() {}

CMyPlayer::CMyPlayer(bs::HSceneObject parent, World *world)
    : Component(parent), m_client(std::make_unique<Client>(world)),
      m_world(world), m_weapon(Creator::Types::kInvalid) {
  setName("CMyPlayer");
  m_fpsWalker = SO()->addComponent<bs::FPSWalker>(world);
  m_fpsWalker->makeActive();
  m_lastRotation = SO()->getTransform().getRotation();
  m_fire1 = bs::VirtualButton("Fire1");
  m_fire2 = bs::VirtualButton("Fire2");
}

void CMyPlayer::setUniqueId(UniqueId uid) { m_client->setUid(uid); }

UniqueId CMyPlayer::getUniqueId() const { return m_client->getUid(); }

bool CMyPlayer::isConnected() const {
  return m_client->GetConnectionState() == ConnectionState::kConnected;
}

void CMyPlayer::connect(const char *address) { m_client->Connect(address); }

void CMyPlayer::disconnect() { m_client->CloseConnection(); }

void CMyPlayer::update() {
  m_client->Poll();

  if (bs::gVirtualInput().isButtonDown(m_fire1) ||
      bs::gVirtualInput().isButtonHeld(m_fire2)) {
    if (isConnected()) {
      onShoot();
    }
  }
}

void CMyPlayer::fixedUpdate() {
  if (!m_world->serverIsActive() &&
      m_client->GetConnectionState() == ConnectionState::kConnected) {
    auto rotation = SO()->getTransform().getRotation();
    const u8 rotChanged = m_lastRotation != rotation;
    if (rotChanged || m_fpsWalker->hasNewInput()) {
      auto &packet = m_client->getPacket();
      packet.ClearPayload();
      packet.SetHeader(PacketHeaderTypes::kPlayerTick);
      auto mw = packet.GetMemoryWriter();
      mw->Write(m_fpsWalker->getPlayerInput());
      mw->Write(rotChanged);
      if (rotChanged) {
        mw->Write(rotation.x);
        mw->Write(rotation.y);
        mw->Write(rotation.z);
        mw->Write(rotation.w);
        m_lastRotation = rotation;
      }
      mw.Finalize();
      m_client->PacketSend(packet, SendStrategy::kUnreliableNoDelay);
    }
  }
}

void CMyPlayer::onShoot() {
  if (m_weapon == Creator::Types::kInvalid) {
    return;
  }
  auto spawnPos = m_world->getPlayerNetComp()->getState().getPosition();
  const auto &forward =
      m_world->getFpsCamera()->getCamera()->getTransform().getForward();
  spawnPos += forward * 0.7f;
  spawnPos += bs::Vector3(0.0f, 1.0f, 0.0f);
  MoveableState state{};
  state.setType(m_weapon);
  state.setPosition(spawnPos);
  state.setRotation(
      m_world->getFpsCamera()->getCamera()->getTransform().getRotation());
  const bs::Vector3 force{forward * 30.0f};
  auto &packet = m_client->getPacket();
  packet.ClearPayload();
  packet.SetHeader(PacketHeaderTypes::kRequestCreate);
  auto mw = packet.GetMemoryWriter();
  mw->Write(state);
  mw->Write(force.x);
  mw->Write(force.y);
  mw->Write(force.z);
  mw.Finalize();
  m_client->PacketSend(packet, SendStrategy::kUnreliable);
}

void CMyPlayer::setWeapon(Creator::Types weapon) { m_weapon = weapon; }

void CMyPlayer::lookupId(UniqueId uid) {
  auto &packet = m_client->getPacket();
  packet.ClearPayload();
  packet.SetHeader(PacketHeaderTypes::kLookup);
  auto mw = packet.GetMemoryWriter();
  mw->Write(uid);
  mw.Finalize();
  m_client->PacketSend(packet, SendStrategy::kUnreliable);
}

bs::RTTITypeBase *CMyPlayer::getRTTIStatic() {
  return CMyPlayerRTTI::instance();
}

bs::RTTITypeBase *CMyPlayer::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
