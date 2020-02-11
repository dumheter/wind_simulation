#include "client.hpp"
#include "log.hpp"
#include "world.hpp"
#include <microprofile/microprofile.h>

namespace wind {

Client::Client(World *world)
    : m_connection(k_HSteamNetConnection_Invalid),
      m_socketInterface(SteamNetworkingSockets()),
      m_connectionState(ConnectionState::kDisconnected), m_world(world),
      m_uid(UniqueId::kInvalid) {}

Client::Client(World *world, ConnectionId activeConnection)
    : m_connection(activeConnection),
      m_socketInterface(SteamNetworkingSockets()),
      m_connectionState(ConnectionState::kConnected), m_world(world),
      m_uid(UniqueId::kInvalid) {}

Client::~Client() { CloseConnection(); }

bool Client::Poll() {
  if (m_connectionState == ConnectionState::kConnected) {
    MICROPROFILE_SCOPEI("client", "poll", MP_YELLOW);
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
    logVeryVerbose("[client] connected");
    m_connectionState = ConnectionState::kConnected;
    return true;
  } else {
    logWarning("[client] failed to connect");
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
    logVeryVerbose("[client] closed connection");
  }
  SetConnectionState(ConnectionState::kDisconnected);
}

SendResult Client::PacketSend(const Packet &packet,
                              const SendStrategy send_strategy) {
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
  logVerbose("[client] TODO handle packet");

  switch (m_packet.GetHeaderType()) {
  case PacketHeaderTypes::kPlayerJoin:
    logVerbose("packet playerjoin TODO");
    break;
  case PacketHeaderTypes::kPlayerLeave:
    logVerbose("packet playerleave TODO");
    break;
  case PacketHeaderTypes::kPlayerUpdate:
    logVerbose("packet playerupdate TODO");
    break;
  case PacketHeaderTypes::kHello:
    logVerbose("packet hello TODO");
    break;
  }
}

void Client::PollSocketStateChanges() {
  MICROPROFILE_SCOPEI("client", "poll socket state changes", MP_YELLOW);

  m_socketInterface->RunCallbacks(this);
}

bool Client::PollIncomingPackets() {
  MICROPROFILE_SCOPEI("client", "poll incoming packets", MP_YELLOW);

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
      logError("[client] could not parse packet, too big [{}/{}]",
               msg->m_cbSize, m_packet.GetPacketCapacity());
    }
    msg->Release();

  } else if (msg_count < 0) {
    if (m_connectionState != ConnectionState::kDisconnected) {
      logVerbose("[client] failed to check for messages, disconnecting");
      CloseConnection();
    }
  }

  return got_packet;
}

void Client::OnSteamNetConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t *status) {
  switch (status->m_info.m_eState) {
  case k_ESteamNetworkingConnectionState_None: {
    // logVerbose("[client] state none");
    break;
  }

  case k_ESteamNetworkingConnectionState_ClosedByPeer: {
    if (status->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
      logWarning("[client] Connection could not be established.");
    } else {
      logInfo("[client] connection closed by peer");
    }

    // cleanup connection
    CloseConnection();
    break;
  }

  case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
    logInfo("[client] connection problem detected locally");

    // cleanup connection
    CloseConnection();
    break;
  }

  case k_ESteamNetworkingConnectionState_Connecting: {
    logVerbose("[client] connecting");
    break;
  }

  case k_ESteamNetworkingConnectionState_Connected: {
    SetConnectionState(ConnectionState::kConnected);
    break;
  }

  default: {
    logWarning("[client] default ??");
  }
  }
}

void Client::SetConnectionState(const ConnectionState connection_state) {
  m_connectionState = connection_state;
  logInfo("[client] {}", connection_state == ConnectionState::kConnected
                             ? "connected"
                             : "disconnected");
}
} // namespace wind
