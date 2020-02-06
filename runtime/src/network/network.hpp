#ifndef NETWORK_HPP_
#define NETWORK_HPP_

#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>
#include "network/side.hpp"
#include "network/client.hpp"
#include "network/server.hpp"
#include "network/net_common.hpp"
#include "network/packet_handler.hpp"
#include <functional>
#include "network/connection_state.hpp"
#include "log.hpp"
#include "common.hpp"

// ========================================================================== //
// Network
// ========================================================================== //

namespace wind {

template<Side side>
class Network
{
  // ============================================================ //
  // Lifetime
  // ============================================================ //
public:
  Network(game::World* world);

  Network(Network&& other);

  Network& operator=(Network&& other);

  ~Network();

  Network(const Network& other) = delete;
  Network& operator=(const Network& other) = delete;

  // ============================================================ //
  // Contants
  // ============================================================ //
public:
  static constexpr u16 kPort = 24812;

  static constexpr f64 kNetTicksPerSec = 64;

  // ============================================================ //
  // Shared Methods
  // ============================================================ //
public:
  void Update();

  /**
   * Server: Broadcast the packet to all active connections.
   * Client: Unicast the packet to the server.
   */
  void PacketBroadcast(const Packet& packet) const;

  /**
   * Server: Broadcast to all active connections but the excluded one.
   * Client: assert(false)
   */
  void PacketBroadcastExclude(const Packet& packet,
                              const ConnectionId exclude_connection) const;

  void NetworkInfo(const std::string_view message) const;

  void Broadcast(const std::string_view message) const;

  /**
   * Server always returns kConnected
   */
  ConnectionState GetConnectionState() const;

  /**
   * Server always returns std::nullopt
   */
  std::optional<SteamNetworkingQuickConnectionStatus> GetConnectionStatus()
    const;

  PacketHandler& GetPacketHandler() { return packet_handler_; }

  // ============================================================ //
  // Client Only Methods
  // ============================================================ //

  void ConnectToServer(u32 ip, u16 port);
  void ConnectToServer(const String& addr);
  void ConnectToServer(const char8* addr);

  void Disconnect();

  std::optional<entt::entity> GetOurPlayerEntity() const;

  std::optional<PlayerData*> GetOurPlayerData() const;

  // ============================================================ //
  // Server Only Methods
  // ============================================================ //

  void StartServer();

  /**
   * Server only, (client always return std::nullopt)
   */
  std::optional<SteamNetworkingQuickConnectionStatus> GetConnectionStatus(
    const ConnectionId connection_id) const;

  std::optional<entt::entity> EntityFromConnection(ConnectionId con) const;

  // ============================================================ //
  // Private Methods
  // ============================================================ //
private:
  void SetupPacketHandler();

  Client* GetClient() const { return static_cast<Client*>(base_); }
  Server* GetServer() const { return static_cast<Server*>(base_); }

  /**
   * Call once at startup.
   */
  static bool InitNetwork();

  /**
   * Call once before closing program.
   */
  static void ShutdownNetwork();

  void SendPlayerList(const ConnectionId connection_id) const;

  void SetOurPlayerEntity(const std::optional<entt::entity> maybe_entity);

  // ============================================================ //
  // Member Variables
  // ============================================================ //
private:
  /**
   * Packet producer
   * Use GetClient and GetServer to access.
   */
  ISteamNetworkingSocketsCallbacks* base_;

  PacketHandler packet_handler_{};

  Packet packet_{ 10000 };

  game::World* world_;
};

// ============================================================ //
// Template Definition
// ============================================================ //

template<Side side>
Network<side>::Network(game::World* world)
  : world_(world)
{
  if (!Network<side>::InitNetwork()) {
    logError("Failed to init network.");
    std::exit(1);
  }
  SetupPacketHandler();
  if constexpr (side == Side::kServer) {
    base_ = new Server(world);
    StartServer();
  } else {
    base_ = new Client(world);
  }
}

template<Side side>
Network<side>::Network(Network&& other)
  : base_(std::move(other.base_))
  , packet_handler_(std::move(other.packet_handler_))
  , packet_(std::move(other.packet_))
  , world_(std::move(other.world_))
{
  other.base_ = nullptr;
  other.world_ = nullptr;
}

template<Side side>
Network<side>&
Network<side>::operator=(Network&& other)
{
  if (this != &other) {
    base_ = std::move(other.base_);
    packet_handler_ = std::move(other.packet_handler_);
    packet_ = std::move(other.packet_);
    world_ = std::move(other.world_);
    other.base_ = nullptr;
    other.world_ = nullptr;
  }
  return *this;
}

template<Side side>
Network<side>::~Network()
{
  if constexpr (side == Side::kServer) {
    auto server = GetServer();
    server->~Server();
  } else {
    auto client = GetClient();
    client->~Client();
  }
  Network<side>::ShutdownNetwork();
}

void
DebugOutputCallback(ESteamNetworkingSocketsDebugOutputType type,
                    const char* msg);

template<Side side>
bool
Network<side>::InitNetwork()
{
  SteamDatagramErrMsg errMsg;
  if (!GameNetworkingSockets_Init(nullptr, errMsg)) {
    logError("GameNetworkingSockets_Init failed.  [{}]", errMsg);
    return false;
  }

  // auto detail_level = ESteamNetworkingSocketsDebugOutputType::
  //   k_ESteamNetworkingSocketsDebugOutputType_Everything;
  auto detail_level = ESteamNetworkingSocketsDebugOutputType::
    k_ESteamNetworkingSocketsDebugOutputType_Important;

  SteamNetworkingUtils()->SetDebugOutputFunction(detail_level,
                                                 DebugOutputCallback);

  return true;
}

void
SleepAndKill();

template<Side side>
void
Network<side>::ShutdownNetwork()
{
  SleepAndKill();
}
}

#endif // NETWORK_HPP_
