#include "client.hpp"
#include <dlog/dlog.hpp>
#include "shared/scene/scene.hpp"
#include "runtime/world.hpp"
#include <microprofile/microprofile.h>

namespace wind {

Client::Client(World *world)
    : m_connection(k_HSteamNetConnection_Invalid),
      m_socketInterface(SteamNetworkingSockets()),
      m_connectionState(ConnectionState::kDisconnected), m_world(world),
      m_uid(UniqueId::invalid()) {}

Client::Client(World *world, ConnectionId activeConnection)
    : m_connection(activeConnection),
      m_socketInterface(SteamNetworkingSockets()),
      m_connectionState(ConnectionState::kConnected), m_world(world),
      m_uid(UniqueId::invalid()) {}

Client::~Client() { CloseConnection(); }

bool Client::Poll() {
  if (m_connectionState != ConnectionState::kDisconnected) {
    MICROPROFILE_SCOPEI("client", "poll", MP_BISQUE);
    PollSocketStateChanges();
    const bool gotPacket = PollIncomingPackets();
    if (gotPacket) {
      handlePacket();
    }
    return gotPacket;
  }
  return false;
}

bool Client::Connect(const SteamNetworkingIPAddr &address) {
  m_connection = m_socketInterface->ConnectByIPAddress(address, 0, nullptr);
  if (m_connection != k_HSteamNetConnection_Invalid) {
    DLOG_VERBOSE("connect initiated");
    SetConnectionState(ConnectionState::kConnecting);
    return true;
  } else {
    DLOG_WARNING("failed to connect");
    return false;
  }
}

bool Client::Connect(const char *address) {
  SteamNetworkingIPAddr addr{};
  if (!addr.ParseString(address)) {
    return false;
  }

  return Connect(addr);
}

void Client::CloseConnection() {
  if (m_connection != k_HSteamNetConnection_Invalid) {
    m_socketInterface->CloseConnection(m_connection, 0, nullptr, false);
    m_connection = k_HSteamNetConnection_Invalid;
    DLOG_VERBOSE("closed connection");
  }
  SetConnectionState(ConnectionState::kDisconnected);
  m_world->onDisconnect();
  m_uid = UniqueId::invalid();
}

SendResult Client::PacketSend(const Packet &packet,
                              const SendStrategy send_strategy) {
  MICROPROFILE_SCOPEI("client", "packetSend", MP_BISQUE1);
  return Common::SendPacket(packet, send_strategy, m_connection,
                            m_socketInterface);
}

std::optional<SteamNetworkingQuickConnectionStatus>
Client::GetConnectionStatus() const {
  SteamNetworkingQuickConnectionStatus status;
  const bool ok =
      m_socketInterface->GetQuickConnectionStatus(m_connection, &status);
  return ok ? std::optional(status) : std::nullopt;
}

void Client::handlePacket() {
  MICROPROFILE_SCOPEI("client", "handlePacket", MP_BISQUE2);
  if (auto header = m_packet.GetHeaderType();
      header == PacketHeaderTypes::kPlayerJoin) {
    DLOG_VERBOSE("packet playerjoin");
    auto mr = m_packet.GetMemoryReader();
    auto uid = mr.Read<UniqueId>();
    auto state = MoveableState{uid};
    m_world->onPlayerJoin(state);
  } else if (header == PacketHeaderTypes::kPlayerLeave) {
    auto mr = m_packet.GetMemoryReader();
    auto uid = mr.Read<UniqueId>();
    m_world->onPlayerLeave(uid);
  } else if (header == PacketHeaderTypes::kServerTick) {
    if (!m_world->serverIsActive()) {
      auto mr = m_packet.GetMemoryReader();
      const u32 count = mr.Read<u32>();
      MoveableState state;
      for (u32 i = 0; i < count; ++i) {
        state = mr.Read<MoveableState>();
        if (state.id != m_uid) {
          m_world->applyMoveableState(state);
        } else {
          m_world->applyMyMoveableState(state);
        }
      }
    }
  } else if (header == PacketHeaderTypes::kPlayerTick) {
    DLOG_WARNING("got a playerTick packet");
  } else if (header == PacketHeaderTypes::kCreate) {
    DLOG_VERBOSE("got a create packet");
    CreateInfo info = PacketParser::Create(m_packet);
    m_world->buildObject(info);
  } else if (header == PacketHeaderTypes::kRequestCreate) {
    DLOG_WARNING("got a requestcreate packet");
  } else if (header == PacketHeaderTypes::kHello) {
    auto mr = m_packet.GetMemoryReader();
    const auto new_uid = mr.Read<UniqueId>();
    DLOG_VERBOSE("changed uid from {} to {}", m_uid.raw(),
               new_uid.raw());
    m_world->netCompChangeUniqueId(m_uid, new_uid);
    m_uid = new_uid;
    if (!m_world->serverIsActive()) {
      auto so = Scene::load(mr.Read<alflib::String>().GetUTF8());
      m_world->setStaticScene(so);
      auto s = mr.Read<alflib::String>();
      so = Scene::load(s.GetUTF8());
      m_world->setDynamicScene(so);
      m_world->scanForNetComps();
    }
  } else if (header == PacketHeaderTypes::kSceneChange) {
    auto mr = m_packet.GetMemoryReader();
    if (!m_world->serverIsActive()) {
      DLOG_VERBOSE("scene change");
      auto scene = mr.Read<alflib::String>();
      m_world->onSceneChange(scene.GetUTF8());
    }
  } else {
    DLOG_ERROR("unknown packet");
  }
}

void Client::PollSocketStateChanges() {
  MICROPROFILE_SCOPEI("client", "poll socket state changes", MP_BISQUE1);

  m_socketInterface->RunCallbacks(this);
}

bool Client::PollIncomingPackets() {
  MICROPROFILE_SCOPEI("client", "poll incoming packets", MP_BISQUE1);

  ISteamNetworkingMessage *msg = nullptr;
  const int msg_count =
      m_socketInterface->ReceiveMessagesOnConnection(m_connection, &msg, 1);

  bool got_packet = false;
  if (msg_count > 0) {
    bool ok = m_packet.SetPacket(static_cast<const u8 *>(msg->m_pData),
                                 msg->m_cbSize);
    if (ok) {
      m_packet.SetFromConnection(msg->m_conn);
      got_packet = true;
    } else {
      DLOG_ERROR("could not parse packet, too big [{}/{}]",
               msg->m_cbSize, m_packet.GetPacketCapacity());
    }
    msg->Release();

  } else if (msg_count < 0) {
    if (m_connectionState != ConnectionState::kDisconnected) {
      DLOG_VERBOSE("failed to check for messages, disconnecting");
      CloseConnection();
    }
  }

  return got_packet;
}

void Client::OnSteamNetConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t *status) {
  switch (status->m_info.m_eState) {
  case k_ESteamNetworkingConnectionState_None: {
    // DLOG_VERBOSE("state none");
    break;
  }

  case k_ESteamNetworkingConnectionState_ClosedByPeer: {
    if (status->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
      DLOG_WARNING("Connection could not be established.");
    } else {
      DLOG_INFO("connection closed by peer");
    }

    // cleanup connection
    CloseConnection();
    break;
  }

  case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
    DLOG_INFO("connection problem detected locally");

    // cleanup connection
    CloseConnection();
    break;
  }

  case k_ESteamNetworkingConnectionState_Connecting: {
    DLOG_VERBOSE("connecting");
    break;
  }

  case k_ESteamNetworkingConnectionState_Connected: {
    if (ConnectionState::kConnecting == m_connectionState) {
      SetConnectionState(ConnectionState::kConnected);
    }
    break;
  }

  default: {
    DLOG_WARNING("default ??");
  }
  }
}

void Client::SetConnectionState(ConnectionState connection_state) {
  m_connectionState = connection_state;
  constexpr auto conStr = [](ConnectionState state) {
    switch (state) {
    case ConnectionState::kConnected:
      return "connected";
    case ConnectionState::kDisconnected:
      return "disconnected";
    case ConnectionState::kConnecting:
      return "connecting";
    }
    return "internal error";
  };
  DLOG_INFO("state: {}", conStr(connection_state));
}
} // namespace wind
