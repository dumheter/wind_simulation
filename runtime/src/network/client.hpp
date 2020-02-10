#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include "common.hpp"
#include "network/net_common.hpp"
#include "network/packet.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "network/connection_state.hpp"
#include "core/macros.hpp"

// ========================================================================== //
// Forward Declarations
// ========================================================================== //

namespace dib::game {
DIB_FORWARD_DECLARE_CLASS(World);
}

// ========================================================================== //
// Client Declaration
// ========================================================================== //

namespace dib {

class Client : public ISteamNetworkingSocketsCallbacks
{
public:
  Client(game::World* world);

  virtual ~Client() final;

  /**
   * Update internal state and check for packets.
   */
  void Poll(bool& got_packet, Packet& packet_out);

  /**
   * The result of the connection attempt will be reported later when polling.
   */
  void Connect(const SteamNetworkingIPAddr& address);

  void CloseConnection();

  SendResult PacketSend(const Packet& packet, const SendStrategy send_strategy);

  ConnectionState GetConnectionState() { return connection_state_; }

  ConnectionId GetConnectionId() const { return connection_; }

  std::optional<SteamNetworkingQuickConnectionStatus> GetConnectionStatus()
    const;

  std::optional<u32> GetOurPlayerEntity() const { return our_player_entity_; }

  void SetOurPlayerEntity(const std::optional<u32> maybe_entity)
  {
    our_player_entity_ = maybe_entity;
  }

private:
  void PollSocketStateChanges();

  bool PollIncomingPackets(Packet& packet_out);

  virtual void OnSteamNetConnectionStatusChanged(
    SteamNetConnectionStatusChangedCallback_t* status) override;

  void SetConnectionState(const ConnectionState connection_state);

private:
  HSteamNetConnection connection_;
  ISteamNetworkingSockets* socket_interface_;
  ConnectionState connection_state_;
  game::World* world_;
  std::optional<u32> our_player_entity_;
};
}
#endif // CLIENT_HPP_
