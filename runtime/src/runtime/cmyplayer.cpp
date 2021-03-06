#include "cmyplayer.hpp"
#include "runtime/network/util.hpp"
#include "runtime/world.hpp"
#include "shared/scene/component_data.hpp"
#include "shared/scene/serializer.hpp"

namespace wind {

CMyPlayer::CMyPlayer()
    : m_world(nullptr), m_fpsWalker(), m_lastRotation(), m_weapon(), m_fire1(),
      m_fire2() {}

CMyPlayer::CMyPlayer(bs::HSceneObject parent, World *world)
    : Component(parent), m_client(std::make_unique<Client>(world)),
      m_world(world), m_weapon(ObjectType::kInvalid) {
  setName("CMyPlayer");
  m_fpsWalker = SO()->addComponent<FPSWalker>();
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
  if (m_weapon == ObjectType::kInvalid) {
    return;
  }

  const auto forward =
      m_world->getFpsCamera()->getCamera()->getTransform().getForward();

  const auto fn = [this, forward](bs::Vector3 spawnPos) {
    MoveableState state{};
    state.position = spawnPos;
    state.rotation =
        m_world->getFpsCamera()->getCamera()->getTransform().getRotation();
    state.vel = forward * m_shootForce;
    state.setRigid(true);
    const bs::Vector3 scale{0.3f, 0.3f, 0.3f};
    Packet &packet = m_client->getPacket();
    RequestCreateInfo info{};
    info.type = m_weapon;
    info.parent = UniqueId::invalid();
    info.scale = scale;
    info.state = state;
    info.components.emplace_back(ComponentData::asRigidbody());
    info.components.emplace_back(ComponentData::asCollider(0.5f, 8.0f));
    info.components.emplace_back(
        ComponentData::asRenderable("res/textures/grid_og.png"));
    info.components.emplace_back(ComponentData::asWindAffectable());
    PacketBuilder::RequestCreate(packet, info);
    m_client->PacketSend(packet, SendStrategy::kUnreliable);
  };

  auto spawnPos = m_world->getPlayerNetComp()->getState().position;

  spawnPos += forward * 0.7f;
  spawnPos += bs::Vector3(0.0f, 1.0f, 0.0f);

  fn(spawnPos);
  // fn(spawnPos + bs::Vector3(0.5f, 0.0f, 0.0f));
  // fn(spawnPos + bs::Vector3(-0.5f, 0.0f, 0.0f));
  // fn(spawnPos + bs::Vector3(1.0f, 0.0f, 0.0f));
  // fn(spawnPos + bs::Vector3(-1.0f, 0.0f, 0.0f));
  // fn(spawnPos + bs::Vector3(0.5f, 0.0f, 0.5f));
  // fn(spawnPos + bs::Vector3(-0.5f, 0.0f, -0.5f));
  // fn(spawnPos + bs::Vector3(1.0f, 0.0f, 1.0f));
  // fn(spawnPos + bs::Vector3(-1.0f, 0.0f, -1.0f));
  // fn(spawnPos + bs::Vector3(0.0f, 0.0f, 0.5f));
  // fn(spawnPos + bs::Vector3(0.0f, 0.0f, -0.5f));
  // fn(spawnPos + bs::Vector3(0.0f, 0.0f, 1.0f));
  // fn(spawnPos + bs::Vector3(0.0f, 0.0f, -1.0f));
}

void CMyPlayer::setWeapon(ObjectType weapon) { m_weapon = weapon; }

bs::RTTITypeBase *CMyPlayer::getRTTIStatic() {
  return CMyPlayerRTTI::instance();
}

bs::RTTITypeBase *CMyPlayer::getRTTI() const { return getRTTIStatic(); }

} // namespace wind
