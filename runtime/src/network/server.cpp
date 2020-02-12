#include "server.hpp"
#include "log.hpp"
#include "world.hpp"
#include <alflib/core/assert.hpp>
#include <microprofile/microprofile.h>

namespace wind {

Server::Server(World *world)
    : m_socketInterface(SteamNetworkingSockets()), m_world(world),
      m_connectionState(ConnectionState::kDisconnected) {
  m_pollGroup = m_socketInterface->CreatePollGroup();
  if (m_pollGroup == k_HSteamNetPollGroup_Invalid) {
    AlfAssert(false, "failed to create poll group");
  }
}

Server::~Server() {

  for (auto [connection, uid] : m_connections) {
    m_socketInterface->CloseConnection(connection, 0, nullptr, false);
  }

  m_socketInterface->DestroyPollGroup(m_pollGroup);
  m_pollGroup = k_HSteamNetPollGroup_Invalid;
  m_socketInterface->CloseListenSocket(m_socket);
}

void Server::Poll() {
  if (m_connectionState == ConnectionState::kConnected) {
    MICROPROFILE_SCOPEI("server", "poll", MP_YELLOW);
    PollSocketStateChanges();
    const bool got_packet = PollIncomingPacket(m_packet);
    if (got_packet) {
      handlePacket(m_packet);
    }
  }
}

void Server::StartServer(const u16 port) {
  SteamNetworkingIPAddr addr{};
  addr.Clear();
  addr.m_port = port;
  m_socket = m_socketInterface->CreateListenSocketIP(addr, 0, nullptr);
  if (m_socket == k_HSteamListenSocket_Invalid) {
    logError("[server] failed to create a listen socket on port {}", port);
  }
  logVerbose("[server] listening on port {}.", port);
  m_connectionState = ConnectionState::kConnected;
  m_world->setupScene();
}

void Server::PacketBroadcast(const Packet &packet,
                             const SendStrategy send_strategy) {
  for (auto [connection, uid] : m_connections) {
    Common::SendPacket(packet, send_strategy, connection, m_socketInterface);
  }
}

void Server::PacketBroadcastExclude(const Packet &packet,
                                    const SendStrategy send_strategy,
                                    const ConnectionId exclude_connection) {
  for (auto [connection, uid] : m_connections) {
    if (connection != exclude_connection) {
      Common::SendPacket(packet, send_strategy, connection, m_socketInterface);
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

void Server::broadcastServerTick(
    const std::unordered_map<UniqueId, HCNetComponent> &netComps) {
  if (netComps.size() > 1 && !m_connections.empty()) {
    m_packet.ClearPayload();
    m_packet.SetHeader(PacketHeaderTypes::kServerTick);
    auto mw = m_packet.GetMemoryWriter();
    mw->Write(netComps.size());
    for (auto [uid, netComp] : netComps) {
      mw->Write(netComp->getState());
    }
    mw.Finalize();
    PacketBroadcast(m_packet, SendStrategy::kUnreliable);
  }
}

void Server::PollSocketStateChanges() {
  MICROPROFILE_SCOPEI("server", "poll socket state changes", MP_YELLOW);

  m_socketInterface->RunCallbacks(this);
}

bool Server::PollIncomingPacket(Packet &packet_out) {
  MICROPROFILE_SCOPEI("server", "poll incoming packets", MP_YELLOW);

  ISteamNetworkingMessage *msg = nullptr;
  const int msg_count =
      m_socketInterface->ReceiveMessagesOnPollGroup(m_pollGroup, &msg, 1);

  bool got_packet = false;
  if (msg_count > 0) {
    bool ok = packet_out.SetPacket(static_cast<const u8 *>(msg->m_pData),
                                   msg->m_cbSize);
    if (ok) {
      for (auto [connection, uid] : m_connections) {
        if (connection == msg->m_conn) {
          packet_out.SetFromConnection(connection);
          got_packet = true;
          break;
        }
      }
      if (!got_packet) {
        logWarning(
            "[server] received packet from unknown connection, dropping it");
        DisconnectConnection(msg->m_conn);
      }
    } else {
      logError("[server] could not parse packet, too big [{}/{}]",
               msg->m_cbSize, packet_out.GetPacketCapacity());
    }

    msg->Release();

  } else if (msg_count < 0) {
    auto maybe_status = GetConnectionStatus(m_socket);
    if (maybe_status &&
        maybe_status->m_eState == k_ESteamNetworkingConnectionState_Connected) {

      logWarning("[server] failed to check for messages, closing connection");
      if (msg != nullptr) {
        m_socketInterface->CloseConnection(m_socket, 0, nullptr, false);
      }
      m_connectionState = ConnectionState::kDisconnected;
    }
  }

  return got_packet;
}

void Server::handlePacket(Packet &packet) {
  logVerbose("[server] got packet: todo handle packet");
}

void Server::OnSteamNetConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t *status) {
  switch (status->m_info.m_eState) {
  case k_ESteamNetworkingConnectionState_None: {
    logVeryVerbose("[server] state none");
    break;
  }

  case k_ESteamNetworkingConnectionState_ProblemDetectedLocally:
    logWarning("[server] Problem detected locally.");
    [[fallthrough]];
  case k_ESteamNetworkingConnectionState_ClosedByPeer: {
    if (status->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
      logInfo(
          "[server] Connection {}, [{}], disconnected with reason [{}], [{}].",
          status->m_hConn, status->m_info.m_szConnectionDescription,
          status->m_info.m_eEndReason, status->m_info.m_szEndDebug);
    } else if (status->m_eOldState !=
               k_ESteamNetworkingConnectionState_Connecting) {
      logWarning("[server] unknown state");
    }

    DisconnectConnection(status->m_hConn);

    break;
  }

  case k_ESteamNetworkingConnectionState_Connecting: {
    logVerbose("[server] Connection from [{}], adding.",
               status->m_info.m_szConnectionDescription);
    if (m_socketInterface->AcceptConnection(status->m_hConn) != k_EResultOK) {
      logWarning("[server] Failed to accept a connection [{}] [{}]",
                 status->m_info.m_szEndDebug,
                 status->m_info.m_szConnectionDescription);
      break;
    }

    if (!m_socketInterface->SetConnectionPollGroup(status->m_hConn,
                                                   m_pollGroup)) {
      m_socketInterface->CloseConnection(status->m_hConn, 0, nullptr, false);
      logWarning("failed to assign connection to poll group");
      break;
    }

    const auto uid = UniqueIdGenerator::next();
    logVerbose("[server] player {} connected", uid.raw());
    auto [it, ok] = m_connections.insert({status->m_hConn, uid});
    if (!ok) {
      logWarning("failed to add connection to map");
      m_socketInterface->CloseConnection(status->m_hConn, 0, nullptr, false);
      break;
    }

    {
      m_packet.ClearPayload();
      m_packet.SetHeader(PacketHeaderTypes::kHello);
      auto mw = m_packet.GetMemoryWriter();
      mw->Write(uid);
      mw.Finalize();
      PacketUnicast(m_packet, SendStrategy::kReliable, status->m_hConn);
    }

    {
      m_packet.ClearPayload();
      m_packet.SetHeader(PacketHeaderTypes::kPlayerJoin);
      auto mw = m_packet.GetMemoryWriter();
      mw->Write(uid);
      mw.Finalize();
      PacketBroadcastExclude(m_packet, SendStrategy::kReliable,
                             status->m_hConn);
    }

    break;
  }

  case k_ESteamNetworkingConnectionState_Connected: {
    // TODO this never happens, why?!
    logVerbose("[server] Connected !!");

    break;
  }

  default: {
    logWarning("[server] default ??");
  }
  }
}

void Server::DisconnectConnection(const HSteamNetConnection connection) {
  UniqueId leaveUid{};

  auto it = m_connections.find(connection);
  if (it != m_connections.end()) {
    logVerbose("[server] disconnecting {}", it->first);
    leaveUid = it->second;
    m_connections.erase(it);
  }

  if (leaveUid) {
    Packet packet{};
    packet.SetHeader(PacketHeaderTypes::kPlayerLeave);
    auto mw = packet.GetMemoryWriter();
    mw->Write(leaveUid);
    mw.Finalize();
    PacketBroadcast(packet, SendStrategy::kReliable);
  } else {
    logWarning("[server] attempted to disconnect a connection, but did not "
               "find it (map size {})",
               m_connections.size());
  }
}
} // namespace wind
