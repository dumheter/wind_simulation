#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "network/client.hpp"
#include "network/connection_id.hpp"
#include "network/connection_state.hpp"
#include "network/net_common.hpp"
#include "network/packet.hpp"
#include "shared/scene/cnet_component.hpp"
#include "shared/types.hpp"
#include <optional>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include <unordered_map>

// ========================================================================== //
// Server Declaration
// ========================================================================== //

namespace wind {

class World;

class Server : public ISteamNetworkingSocketsCallbacks {
public:
  explicit Server(World *world);

  virtual ~Server() final;

  /**
   * Let the server update, call continuously.
   */
  void Poll();

  /**
   * Start a server on the given port.
   */
  void StartServer(const u16 port);

  void StopServer();

  /**
   * Attempt to close the connection and remove it from our list of connections.
   */
  void DisconnectConnection(ConnectionId connection);

  /**
   * Broadcast a packet to all active connections.
   */
  void PacketBroadcast(const Packet &packet, const SendStrategy send_strategy);

  void PacketBroadcastUnreliableFast(const Packet &packet);

  /**
   * Broadcast, but exclude the single connection given.
   */
  void PacketBroadcastExclude(const Packet &packet,
                              const SendStrategy send_strategy,
                              const ConnectionId exclude_connection);

  /**
   * Send a packet to a connection.
   */
  SendResult PacketUnicast(const Packet &packet,
                           const SendStrategy send_strategy,
                           const ConnectionId target_connection);

  std::optional<SteamNetworkingQuickConnectionStatus>
  GetConnectionStatus(const ConnectionId connection_id) const;

  ConnectionState getConnectionState() const { return m_connectionState; }

  bool isActive() const {
    return m_connectionState == ConnectionState::kConnected;
  }

  void broadcastServerTick(
      const std::unordered_map<UniqueId, HCNetComponent> &netComps);

  Packet &getPacket() { return m_packet; }
  const Packet &getPacket() const { return m_packet; }

private:
  void PollSocketStateChanges();

  bool PollIncomingPacket(Packet &packet_out);

  void handlePacket();

  void handlePacketPlayerTick();

  void handlePacketRequestCreate();

  virtual void OnSteamNetConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t *status) override;

private:
  HSteamListenSocket m_socket;
  ISteamNetworkingSockets *m_socketInterface;
  HSteamNetPollGroup m_pollGroup;
  std::unordered_map<ConnectionId, UniqueId> m_connections;
  World *m_world;
  Packet m_packet{10000000};
  ConnectionState m_connectionState;
};
} // namespace wind

#endif // SERVER_HPP_
