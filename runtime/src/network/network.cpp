#include "log.hpp"
#include <alflib/core/assert.hpp>
#include <chrono>
#include <functional>
#include <limits>
#include <microprofile/microprofile.h>
#include <thread>

namespace wind {

template <>
void Network<Side::kServer>::PacketBroadcast(const Packet &packet) const {
  auto server = GetServer();
  server->PacketBroadcast(packet, SendStrategy::kUnreliableNoNagle);
}

template <>
void Network<Side::kClient>::PacketBroadcast(const Packet &packet) const {
  auto client = GetClient();
  const auto res = client->PacketSend(packet, SendStrategy::kUnreliableNoNagle);
  if (res != SendResult::kSuccess) {
    // TODO handle this fail
    logError("failed to send packet from client, fail not handled");
  }
}

template <>
void Network<Side::kServer>::PacketBroadcastExclude(
    const Packet &packet, const ConnectionId exclude_connection) const {
  auto server = GetServer();
  server->PacketBroadcastExclude(packet, SendStrategy::kUnreliableNoNagle,
                                 exclude_connection);
}

template <>
void Network<Side::kClient>::PacketBroadcastExclude(const Packet &,
                                                    const ConnectionId) const {
  AlfAssert(false, "cannot broadcastExclude from client");
}

template <> void Network<Side::kServer>::ConnectToServer(u32, u16) {
  AlfAssert(false, "cannot connect to server from server");
}

template <> void Network<Side::kClient>::ConnectToServer(u32 ip, u16 port) {
  SteamNetworkingIPAddr addr{};
  addr.SetIPv4(ip, port);
  auto client = GetClient();
  client->Connect(addr);
}

template <> void Network<Side::kServer>::ConnectToServer(const String &) {
  AlfAssert(false, "cannot connect to server from server");
}

template <> void Network<Side::kClient>::ConnectToServer(const String &addr) {
  SteamNetworkingIPAddr saddr{};
  saddr.ParseString(addr.GetUTF8());
  auto client = GetClient();
  client->Connect(saddr);
}

template <> void Network<Side::kServer>::ConnectToServer(const char8 *) {
  AlfAssert(false, "cannot connect to server from server");
}

template <> void Network<Side::kClient>::ConnectToServer(const char8 *addr) {
  SteamNetworkingIPAddr saddr{};
  saddr.ParseString(addr);
  auto client = GetClient();
  client->Connect(saddr);
}

template <> void Network<Side::kServer>::Disconnect() {
  AlfAssert(false, "cannot disconnect on the server");
}

template <> void Network<Side::kClient>::Disconnect() {
  auto client = GetClient();
  client->CloseConnection();
}

template <> void Network<Side::kServer>::StartServer() {
  auto server = GetServer();
  server->StartServer(kPort);
}

template <> void Network<Side::kClient>::StartServer() {
  AlfAssert(false, "cannot start server on client");
}

template <>
std::optional<entt::entity> Network<Side::kServer>::GetOurPlayerEntity() const {
  AlfAssert(false, "cannot get our player entity on server");
  return std::nullopt;
}

template <>
std::optional<entt::entity> Network<Side::kClient>::GetOurPlayerEntity() const {
  auto client = GetClient();
  return client->GetOurPlayerEntity();
}

template <>
void Network<Side::kServer>::SetOurPlayerEntity(
    const std::optional<entt::entity>) {
  AlfAssert(false, "cannot set our player entity on server");
}

template <>
void Network<Side::kClient>::SetOurPlayerEntity(
    const std::optional<entt::entity> maybe_entity) {
  auto client = GetClient();
  client->SetOurPlayerEntity(maybe_entity);
}

template <>
std::optional<PlayerData *> Network<Side::kServer>::GetOurPlayerData() const {
  AlfAssert(false, "cannot get our PlayerData on server");
  return std::nullopt;
}

template <>
std::optional<PlayerData *> Network<Side::kClient>::GetOurPlayerData() const {
  auto &registry = world_->GetEntityManager().GetRegistry();
  if (const auto maybe_entity = GetOurPlayerEntity(); maybe_entity) {
    PlayerData *pd = &registry.get<PlayerData>(*maybe_entity);
    return pd;
  }
  return std::nullopt;
}

template <>
std::optional<entt::entity>
Network<Side::kServer>::EntityFromConnection(const ConnectionId con) const {
  auto &registry = world_->GetEntityManager().GetRegistry();
  const auto view = registry.view<PlayerData>();
  for (const auto entity : view) {
    if (view.get(entity).connection_id == con) {
      return entity;
    }
  }
  return std::nullopt;
}

template <>
std::optional<entt::entity>
Network<Side::kClient>::EntityFromConnection(const ConnectionId) const {
  return std::nullopt;
}

template <>
std::optional<SteamNetworkingQuickConnectionStatus>
Network<Side::kServer>::GetConnectionStatus(
    const ConnectionId connection_id) const {
  auto server = GetServer();
  return server->GetConnectionStatus(connection_id);
}

template <>
std::optional<SteamNetworkingQuickConnectionStatus>
Network<Side::kClient>::GetConnectionStatus([
    [maybe_unused]] const ConnectionId connection_id) const {
  AlfAssert(false,
            "cannot call GetConnectionStatus(connection_id) from client");
  return std::nullopt;
}

template <>
std::optional<SteamNetworkingQuickConnectionStatus>
Network<Side::kServer>::GetConnectionStatus() const {
  AlfAssert(false, "cannot call GetConnectionStatus() from server");
  return std::nullopt;
}

template <>
std::optional<SteamNetworkingQuickConnectionStatus>
Network<Side::kClient>::GetConnectionStatus() const {
  auto client = GetClient();
  return client->GetConnectionStatus();
}

template <>
void Network<Side::kServer>::NetworkInfo([
    [maybe_unused]] const std::string_view message) const {
  world_->GetEntityManager().GetRegistry().view<PlayerData>().each(
      [](const PlayerData &player_data) {
        // DLOG_RAW("\tConnection: {}\n\tPlayer: [{}]\n",
        //          player_data.connection_id, player_data);
      });
}

template <>
void Network<Side::kClient>::NetworkInfo([
    [maybe_unused]] const std::string_view message) const {
  world_->GetEntityManager().GetRegistry().view<PlayerData>().each(
      [](const PlayerData &player_data) {
        // DLOG_RAW("\tPlayer: [{}]\n", player_data);
      });
}

template <>
void Network<Side::kServer>::SendPlayerList(
    const ConnectionId connection_id) const {
  Packet packet{};
  packet_handler_.BuildPacketHeader(packet,
                                    PacketHeaderStaticTypes::kPlayerJoin);

  world_->GetEntityManager()
      .GetRegistry()
      .view<PlayerData, game::Moveable>()
      .each([&](const PlayerData &player_data, const game::Moveable &moveable) {
        if (connection_id != player_data.connection_id) {
          packet.ClearPayload();
          auto mw = packet.GetMemoryWriter();
          mw->Write(player_data);
          mw->Write(moveable);
          mw.Finalize();
          auto server = GetServer();
          server->PacketUnicast(packet, SendStrategy::kUnreliableNoNagle,
                                connection_id);
        }
      });
}

template <>
void Network<Side::kClient>::SendPlayerList(const ConnectionId) const {
  AlfAssert(false, "cannot send player list from client");
}

// ============================================================ //
// SetupPacketHandler CLIENT
// ============================================================ //

template <> void Network<Side::kClient>::SetupPacketHandler() {

  const auto SyncCb = [&](const Packet &packet) {
    // packet_handler_.OnPacketSync(packet);
    logVerbose("packet sync");
  };
  bool ok = packet_handler_.AddStaticPacketType(PacketHeaderStaticTypes::kSync,
                                                "sync", SyncCb);
  AlfAssert(ok, "could not add packet type sync");

  // ============================================================ //

  const auto PlayerJoinCb = [this](const Packet &packet) {
    logVerbose("packet playerjoin");
  };
  ok = packet_handler_.AddStaticPacketType(PacketHeaderStaticTypes::kPlayerJoin,
                                           "player join", PlayerJoinCb);
  AlfAssert(ok, "could not add packet type player join");

  // ============================================================ //

  const auto PlayerLeaveCb = [&](const Packet &packet) {
    // auto mr = packet.GetMemoryReader();
    // const auto uuid = mr.Read<Uuid>();

    logVerbose("player leave")

    // system::Delete<PlayerData>(registry, uuid);
  };
  ok = packet_handler_.AddStaticPacketType(
      PacketHeaderStaticTypes::kPlayerLeave, "player leave", PlayerLeaveCb);
  AlfAssert(ok, "could not add packet type player leave");

  // ============================================================ //

  const auto PlayerUpdateCb = [this](const Packet &packet) {
    // auto mr = packet.GetMemoryReader();
    // auto player_data = mr.Read<PlayerData>();

    logVerbose("player update");
  };
  ok = packet_handler_.AddStaticPacketType(
      PacketHeaderStaticTypes::kPlayerUpdate, "player update", PlayerUpdateCb);
  AlfAssert(ok, "could not add packet type player update");

  // ============================================================ //

  const auto PlayerInputCb = [](const Packet &) {
    logWarning("got a PlayerInput packet, but server should "
               "not send those, ignoring it");
  };
  ok = packet_handler_.AddStaticPacketType(
      PacketHeaderStaticTypes::kPlayerInput, "player input", PlayerInputCb);
  AlfAssert(ok, "could not add packet type player input");
}

// ============================================================ //
// SetupPacketHandler SERVER
// ============================================================ //

template <> void Network<Side::kServer>::SetupPacketHandler() {

  const auto PlayerJoinCb = [&](const Packet &packet) {
    logVerbose("playe join");
  };
  ok = packet_handler_.AddStaticPacketType(PacketHeaderStaticTypes::kPlayerJoin,
                                           "player join", PlayerJoinCb);
  AlfAssert(ok, "could not add packet type player join");

  // ============================================================ //

  const auto PlayerLeaveCb = [](const Packet &) { logVerbose("player leave"); };
  ok = packet_handler_.AddStaticPacketType(
      PacketHeaderStaticTypes::kPlayerLeave, "player leave", PlayerLeaveCb);
  AlfAssert(ok, "could not add packet type player leave");

  // ============================================================ //

  const auto PlayerUpdateCb = [&](const Packet &packet) {
    // auto mr = packet.GetMemoryReader();
    // auto player_data = mr.Read<PlayerData>();

    logVerbose("player update");
  };
  ok = packet_handler_.AddStaticPacketType(
      PacketHeaderStaticTypes::kPlayerUpdate, "player update", PlayerUpdateCb);
  AlfAssert(ok, "could not add packet type player update");
}

template <> void Network<Side::kClient>::Update() {
  MICROPROFILE_SCOPEI("network", "update", MP_YELLOW);

  // update network
  static bool got_update;
  auto client = GetClient();
  static const auto PollClient =
      std::bind(&Client::Poll, client, std::ref(got_update), std::ref(packet_));
  got_update = false;
  // TODO
  // dutil::FixedTimeUpdate(kNetTicksPerSec, PollClient);
  while (got_update) {
    bool success = packet_handler_.HandlePacket(packet_);
    if (!success) {
      logWarning("Could not handle packet on client");
    }
    PollClient(got_update, packet_);
  }
}

template <> void Network<Side::kServer>::Update() {
  MICROPROFILE_SCOPEI("network", "update", MP_YELLOW);

  // update network
  static bool got_update = false; // needs to be static?
  auto server = GetServer();
  static const auto PollServer =
      std::bind(&Server::Poll, server, std::ref(got_update), std::ref(packet_));
  got_update = false;
  // TODO
  // dutil::FixedTimeUpdate(kNetTicksPerSec, PollServer);
  while (got_update) {
    bool success = packet_handler_.HandlePacket(packet_);
    if (!success) {
      logWarning("Could not handle packet on server");
    }
    PollServer(got_update, packet_);
  }
}

template <>
void Network<Side::kServer>::Broadcast(const std::string_view message) const {
  game::ChatMessage msg{};
  msg.type = game::ChatType::kServer;
  msg.msg = String(message.data());
  world_->GetChat().FillFromTo(msg);
  if (!world_->GetChat().SendMessage(msg)) {
    logWarning("failed to broadcast a server chat message");
  }
}

template <>
void Network<Side::kClient>::Broadcast([
    [maybe_unused]] const std::string_view message) const {}

template <> ConnectionState Network<Side::kServer>::GetConnectionState() const {
  return ConnectionState::kConnected;
}

template <> ConnectionState Network<Side::kClient>::GetConnectionState() const {
  auto client = GetClient();
  return client->GetConnectionState();
}

// ============================================================ //

void DebugOutputCallback(ESteamNetworkingSocketsDebugOutputType type,
                         const char *msg) {
  logVerbose("Steam Socket debug output: [{}] [{}]", type, msg);
}

void SleepAndKill() {
  // Give the sockets time to finish up, not sure if needed.
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  GameNetworkingSockets_Kill();
}

// ============================================================ //

} // namespace wind
