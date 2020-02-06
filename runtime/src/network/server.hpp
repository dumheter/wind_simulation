#ifndef SERVER_HPP_
#define SERVER_HPP_

#include "common.hpp"
#include "network/net_common.hpp"
#include "network/packet.hpp"
#include "network/side.hpp"
#include "network/connection_id.hpp"
#include "network/connection_state.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include <tsl/robin_set.h>
#include <optional>

// ========================================================================== //
// Forward Declarations
// ========================================================================== //

namespace dib::game {
DIB_FORWARD_DECLARE_CLASS(World);
}

// ========================================================================== //
// Server Declaration
// ========================================================================== //

namespace dib {

class Server : public ISteamNetworkingSocketsCallbacks
{
public:
  Server(game::World* world);

  virtual ~Server() final;

  /**
   * Let the server update, call continuously.
   */
  void Poll(bool& got_packet, Packet& packet_out);

  /**
   * Start a server on the given port.
   */
  void StartServer(const u16 port);

  /**
   * Attempt to close the connection and remove it from our list of connections.
   */
  void DisconnectConnection(const HSteamNetConnection connection);

  /**
   * Broadcast a packet to all active connections.
   */
  void PacketBroadcast(const Packet& packet, const SendStrategy send_strategy);

  /**
   * Broadcast, but exclude the single connection given.
   */
  void PacketBroadcastExclude(const Packet& packet,
                              const SendStrategy send_strategy,
                              const ConnectionId exclude_connection);

  /**
   * Send a packet to a connection.
   */
  SendResult PacketUnicast(const Packet& packet,
                           const SendStrategy send_strategy,
                           const HSteamNetConnection target_connection);

  NetworkState GetNetworkState() const { return network_state_; }

  std::optional<SteamNetworkingQuickConnectionStatus> GetConnectionStatus(
    const ConnectionId connection_id) const;

private:
  void PollSocketStateChanges();

  bool PollIncomingPackets(Packet& packet_out);

  virtual void OnSteamNetConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* status) override;

  /**
   * Let steam sockets close the connection. Does not remove it from our
   * list of active connections, use DisconnectConnection for that.
   */
  void CloseConnection(HSteamNetConnection connection);

private:
  HSteamListenSocket socket_;
  ISteamNetworkingSockets* socket_interface_;
  tsl::robin_set<ConnectionId> connections_{};
  NetworkState network_state_ = NetworkState::kServer;
  game::World* world_;
};
}

#endif // SERVER_HPP_
