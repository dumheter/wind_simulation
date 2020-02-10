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
  MICROPROFILE_SCOPEI("client", "poll", MP_YELLOW);
  PollSocketStateChanges();
  const bool gotPacket = PollIncomingPackets();
  handlePacket();
  return gotPacket;
}

void Client::Connect(const SteamNetworkingIPAddr &address) {
  m_connection = m_socketInterface->ConnectByIPAddress(address, 0, nullptr);
  if (m_connection == k_HSteamNetConnection_Invalid) {
    logWarning("failed to connect");
  } else {
    logVeryVerbose("connected");
  }
}

void Client::CloseConnection() {
  if (m_connection != k_HSteamNetConnection_Invalid) {
    SetConnectionState(ConnectionState::kDisconnected);
    m_socketInterface->CloseConnection(m_connection, 0, nullptr, false);
    m_connection = k_HSteamNetConnection_Invalid;
    logVeryVerbose("closed connection");
  }

  // m_player->destroy();
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

void Client::handlePacket() { logVerbose("TODO handle packet"); }

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
      logError("could not parse packet, too big [{}/{}]", msg->m_cbSize,
               m_packet.GetPacketCapacity());
    }
    msg->Release();

  } else if (msg_count < 0) {
    if (m_connectionState != ConnectionState::kDisconnected) {
      logVerbose("failed to check for messages, disconnecting");
      CloseConnection();
    }
  }

  return got_packet;
}

void Client::OnSteamNetConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t *status) {
  switch (status->m_info.m_eState) {
  case k_ESteamNetworkingConnectionState_None: {
    // logVerbose("state none");
    break;
  }

  case k_ESteamNetworkingConnectionState_ClosedByPeer: {
    if (status->m_eOldState == k_ESteamNetworkingConnectionState_Connecting) {
      logWarning("Connection could not be established.");
    } else {
      logInfo("connection closed by peer");
    }

    // cleanup connection
    CloseConnection();
    break;
  }

  case k_ESteamNetworkingConnectionState_ProblemDetectedLocally: {
    logInfo("connection problem detected locally");

    // cleanup connection
    CloseConnection();
    break;
  }

  case k_ESteamNetworkingConnectionState_Connecting: {
    logVerbose("connecting");
    break;
  }

  case k_ESteamNetworkingConnectionState_Connected: {
    logInfo("connected");
    SetConnectionState(ConnectionState::kConnected);
    break;
  }

  default: {
    logWarning("default ??");
  }
  }
}

void Client::SetConnectionState(const ConnectionState connection_state) {
  m_connectionState = connection_state;
  logInfo("{}", connection_state == ConnectionState::kConnected
                    ? "connected"
                    : "disconnected");
}
} // namespace wind