#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include "BsApplication.h"
#include "network/connection_state.hpp"
#include "network/net_common.hpp"
#include "network/packet.hpp"
#include "shared/types.hpp"
#include "shared/utility/unique_id.hpp"
#include <optional>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

namespace wind {

class World;

class Client : public ISteamNetworkingSocketsCallbacks {
public:
  Client() = default;
  explicit Client(World *world);
  Client(World *world, ConnectionId activeConnection);

  virtual ~Client() final;

  /**
   * Update internal state and check for packets.
   * Also handle packets.
   */
  bool Poll();

  /**
   * The result of the connection attempt will be reported later when polling.
   */
  bool Connect(const SteamNetworkingIPAddr &address);
  bool Connect(const char *address);

  void CloseConnection();

  void setUid(UniqueId uid) { m_uid = uid; }

  UniqueId getUid() const { return m_uid; }

  void generateUid() { m_uid = UniqueIdGenerator::next(); }

  SendResult PacketSend(const Packet &packet, const SendStrategy send_strategy);

  ConnectionState GetConnectionState() const { return m_connectionState; }

  ConnectionId getConnectionId() const { return m_connection; }

  std::optional<SteamNetworkingQuickConnectionStatus>
  GetConnectionStatus() const;

  bool operator==(const Client &other) const {
    return m_connection == other.m_connection;
  }

  bool operator!=(const Client &other) const { return !(*this == other); }

  Packet &getPacket() { return m_packet; }

private:
  void handlePacket();

  void PollSocketStateChanges();

  bool PollIncomingPackets();

  virtual void OnSteamNetConnectionStatusChanged(
      SteamNetConnectionStatusChangedCallback_t *status) override;

  void SetConnectionState(const ConnectionState connection_state);

private:
  ConnectionId m_connection;
  ISteamNetworkingSockets *m_socketInterface;
  ConnectionState m_connectionState;
  World *m_world;
  UniqueId m_uid;
  Packet m_packet{10000000};
};

} // namespace wind

#endif // CLIENT_HPP_
