#include "server.hpp"
#include "log.hpp"
#include "world.hpp"
#include <microprofile/microprofile.h>

namespace wind {

Server::Server(World *world)
    : m_socketInterface(SteamNetworkingSockets()), m_world(world), m_connectionState(ConnectionState::kDisconnected) {}

Server::~Server() {
  m_socketInterface->CloseListenSocket(m_socket);
  for (auto &client : m_clients) {
    client.CloseConnection();
  }
}

void Server::Poll() {
  MICROPROFILE_SCOPEI("server", "poll", MP_YELLOW);
  PollSocketStateChanges();
  const bool got_packet = PollIncomingPacket(m_packet);
  if (got_packet) {
    handlePacket(m_packet);
  }
}

void Server::StartServer(const u16 port) {
  SteamNetworkingIPAddr addr{};
  addr.Clear();
  addr.m_port = port;
  m_socket = m_socketInterface->CreateListenSocketIP(addr, 0, nullptr);
  if (m_socket == k_HSteamListenSocket_Invalid) {
    logError("failed to create a listen socket on port {}", port);
  }
  logVerbose("listening on port {}.", port);
  m_connectionState = ConnectionState::kConnected;
}

void Server::PacketBroadcast(const Packet &packet,
                             const SendStrategy send_strategy) {
  for (auto &client : m_clients) {
    Common::SendPacket(packet, send_strategy, client.getConnectionId(),
                       m_socketInterface);
  }
}

void Server::PacketBroadcastExclude(const Packet &packet,
                                    const SendStrategy send_strategy,
                                    const ConnectionId exclude_connection) {
  for (auto &client : m_clients) {
    if (client.getConnectionId() != exclude_connection) {
      Common::SendPacket(packet, send_strategy, client.getConnectionId(),
                         m_socketInterface);
    }
  }
}

SendResult Server::PacketUnicast(const Packet &packet,
                                 const SendStrategy send_strategy,
                                 const ConnectionId target_connection) {
  return Common::SendPacket(packet, send_strategy, target_connection,
                            m_socketInterface);
}

std::optional<SteamNetworkingQuickConnectionStatus>
Server::GetConnectionStatus(const ConnectionId connection_id) const {
  SteamNetworkingQuickConnectionStatus status;
  const bool ok =
      m_socketInterface->GetQuickConnectionStatus(connection_id, &status);
  return ok ? std::optional(status) : std::nullopt;
}

void Server::PollSocketStateChanges() {
  MICROPROFILE_SCOPEI("server", "poll socket state changes", MP_YELLOW);

  m_socketInterface->RunCallbacks(this);
}

bool Server::PollIncomingPacket(Packet &packet_out) {
  MICROPROFILE_SCOPEI("server", "poll incoming packets", MP_YELLOW);

  ISteamNetworkingMessage *msg = nullptr;
  const int msg_count =
      m_socketInterface->ReceiveMessagesOnConnection(m_socket, &msg, 1);

  bool got_packet = false;
  if (msg_count > 0) {
    bool ok = packet_out.SetPacket(static_cast<const u8 *>(msg->m_pData),
                                   msg->m_cbSize);
    if (ok) {
      for (auto &client : m_clients) {
        if (client.getConnectionId() == msg->m_conn) {
          packet_out.SetFromConnection(client.getConnectionId());
          got_packet = true;
          break;
        }
      }
      if (!got_packet) {
        logWarning("received packet from unknown connection, dropping it");
        DisconnectConnection(msg->m_conn);
      }
    } else {
      logError("could not parse packet, too big [{}/{}]", msg->m_cbSize,
               packet_out.GetPacketCapacity());
    }

    msg->Release();

  } else if (msg_count < 0) {
    logWarning("failed to check for messages, closing connection");
    if (msg != nullptr) {
      DisconnectConnection(msg->m_conn);
    }
  }

  return got_packet;
}

void Server::handlePacket(Packet &packet) { logVerbose("todo handle packet"); }

void Server::OnSteamNetConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t *status) {
  switch (status->m_info.m_eState) {
  case k_ESteamNetworkingConnectionState_None: {
    logVeryVerbose("state none");
    break;
  }

  case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    logWarning("Problem detected locally.");
    [[fallthrough]];
  case k_ESteamNetworkingConnectionState_ClosedByPeer: {
    if (status->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
      logInfo("Connection {}, [{}], disconnected with reason [{}], [{}].",
              status->m_hConn, status->m_info.m_szConnectionDescription,
              status->m_info.m_eEndReason, status->m_info.m_szEndDebug);
    } else if (status->m_eOldState !=
               k_ESteamNetworkingConnectionState_Connecting) {
      logWarning("unknown state");
    }

    DisconnectConnection(status->m_hConn);

    break;
  }

  case k_ESteamNetworkingConnectionState_Connecting: {
    logVerbose("Connection from [{}], adding.",
               status->m_info.m_szConnectionDescription);
    if (m_socketInterface->AcceptConnection(status->m_hConn) != k_EResultOK) {
      logWarning("Failed to accept a connection");
      break;
    }

    m_clients.push_back(Client{m_world, status->m_hConn});

    Packet packet{};
    packet.SetHeader(PacketHeaderTypes::kPlayerJoin);
    auto mw = packet.GetMemoryWriter();
    mw->Write(m_clients.back().getUid());
    mw.Finalize();
    PacketBroadcast(packet, SendStrategy::kReliable);

    m_world->onPlayerJoin(m_clients.back().getUid());

    break;
  }

  case k_ESteamNetworkingConnectionState_Connected: {
    logVerbose("connected");

    // Send a sync packet to the connection
    // Packet packet{};
    // const auto res =
    //     PacketUnicast(packet, SendStrategy::kReliable, status->m_hConn);
    // if (res != SendResult::kSuccess) {
    //   logWarning("failed to send sync packet to {}, disconnecting them.",
    //              status->m_hConn);
    //   DisconnectConnection(status->m_hConn);
    // }
    break;
  }

  default: {
    logWarning("default ??");
  }
  }
}

void Server::DisconnectConnection(const HSteamNetConnection connection) {
  UniqueId leaveUid{};

  for (auto client = m_clients.begin(); client != m_clients.end(); ++client) {
    if (client->getConnectionId() == connection) {
      leaveUid = client->getUid();
      m_clients.erase(client);
      break;
    }
  }

  if (leaveUid) {
    Packet packet{};
    packet.SetHeader(PacketHeaderTypes::kPlayerLeave);
    auto mw = packet.GetMemoryWriter();
    mw->Write(leaveUid);
    mw.Finalize();
    PacketBroadcast(packet, SendStrategy::kReliable);
  } else {
    logWarning("attempted to disconnect a connection, but did not find it");
  }
}
} // namespace wind
