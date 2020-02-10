#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "common.hpp"
#include "network/client.hpp"
#include "network/connection_id.hpp"
#include "network/connection_state.hpp"
#include "network/net_common.hpp"
#include "network/packet.hpp"
#include "network/side.hpp"
#include <optional>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include <vector>

// ========================================================================== //
// Server Declaration
// ========================================================================== //

namespace wind {

class World;

class Server : public ISteamNetworkingSocketsCallbacks {
public:
  Server(World *world);

  virtual ~Server() final;

  /**
   * Let the server update, call continuously.
   */
  void Poll();

  /**
   * Start a server on the given port.
   */
  void StartServer(const u16 port);

  /**
   * Attempt to close the connection and remove it from our list of connections.
   */
  void DisconnectConnection(const ConnectionId connection);

  /**
   * Broadcast a packet to all active connections.
   */
  void PacketBroadcast(const Packet &packet, const SendStrategy send_strategy);

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

private:
  void PollSocketStateChanges();

  bool PollIncomingPacket(Packet &packet_out);

  void handlePacket(Packet &packet);

  virtual void OnSteamNetConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t *status) override;

  /**
   * Let steam sockets close the connection. Does not remove it from our
   * list of active connections, use DisconnectConnection for that.
   */
  void CloseConnection(ConnectionId connection);

private:
  HSteamListenSocket m_socket;
  ISteamNetworkingSockets *m_socketInterface;
  std::vector<Client> m_clients{};
  World *m_world;
  Packet m_packet{10000};
};
} // namespace wind

#endif // SERVER_HPP_