#include "net_common.hpp"
#include "log.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

namespace wind::Common {

SendResult SendPacket(const Packet &packet, const SendStrategy send_strategy,
                      const HSteamNetConnection connection,
                      ISteamNetworkingSockets *socket_interface) {
  const EResult res = socket_interface->SendMessageToConnection(
      connection, packet.GetPacket(), static_cast<u32>(packet.GetPacketSize()),
      static_cast<int>(send_strategy), nullptr);

  SendResult result = SendResult::kSuccess;
  if (res != EResult::k_EResultOK) {

    switch (res) {
    case k_EResultInvalidParam:
      logWarning(
          "invalid connection handle, or the individual message is too big.");
      result = SendResult::kReconnect;
      if (packet.GetPacketSize() >
          k_cbMaxSteamNetworkingSocketsMessageSizeSend) {
        logError("packet size is too big to send, this case is not handled");
      }
      break;

    case k_EResultInvalidState:
      logWarning("connection is in an invalid state");
      result = SendResult::kReconnect;
      break;

    case k_EResultNoConnection:
      logWarning("attempted to send on a closed connection");
      result = SendResult::kReconnect;
      break;

    case k_EResultIgnored:
      logWarning("You used k_nSteamNetworkingSend_NoDelay, and the message "
                 "was dropped because");
      result = SendResult::kRetry;
      break;

    case k_EResultLimitExceeded:
      logWarning("there was already too much data queued to be sent.");
      result = SendResult::kRetry;
      break;

    default:
      logError("default case should never happen");
      result = SendResult::kReconnect;
    }
  }

  return result;
}

static void DebugOutputCallback(ESteamNetworkingSocketsDebugOutputType type,
                                const char *msg) {
  logInfo("[STEAM:{}] {}", type, msg);
}

void InitNetwork() {
  SteamDatagramErrMsg errMsg;
  if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
    logError("failed to init network: [{}]", errMsg);
  }
  // auto detail_level = ESteamNetworkingSocketsDebugOutputType::
  //   k_ESteamNetworkingSocketsDebugOutputType_Everything;
  auto detail_level = ESteamNetworkingSocketsDebugOutputType::
      k_ESteamNetworkingSocketsDebugOutputType_Important;
  SteamNetworkingUtils()->SetDebugOutputFunction(detail_level,
                                                 DebugOutputCallback);
}

void KillNetwork() { GameNetworkingSockets_Kill(); }

} // namespace wind::Common
