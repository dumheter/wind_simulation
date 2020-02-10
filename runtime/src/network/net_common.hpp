#ifndef NET_COMMON_HPP_
#define NET_COMMON_HPP_

#include "common.hpp"
#include "network/packet.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

namespace wind {

enum class NetworkState { kClientOffline = 0, kServer, kClientOnline };

enum class SendResult { kSuccess = 0, kReconnect, kRetry };

enum class SendStrategy : int {
  kReliable = k_nSteamNetworkingSend_Reliable,
  kUnreliable = k_nSteamNetworkingSend_Unreliable,

  /**
   * Special use strategies only.
   */
  kReliableNoNagle = k_nSteamNetworkingSend_ReliableNoNagle,
  kUnreliableNoNagle = k_nSteamNetworkingSend_UnreliableNoNagle,
  kUnreliableNoDelay = k_nSteamNetworkingSend_UnreliableNoDelay
};

namespace Common {

SendResult SendPacket(const Packet &packet, const SendStrategy send_strategy,
                      const HSteamNetConnection connection,
                      ISteamNetworkingSockets *socket_interface);

}

} // namespace wind

#endif // NET_COMMON_HPP_
