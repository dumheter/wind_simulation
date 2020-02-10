#ifndef PACKET_HANDLER_HPP_
#define PACKET_HANDLER_HPP_

#include "network/packet.hpp"
#include <functional>
#include <string_view>
#include <optional>
#include <array>
#include <unordered_map>

namespace dib {

// ============================================================ //
// Predefined packet types
// ============================================================ //

/**
 * Steps to follow when ADDING a new packet type.
 * 1. Add the enum here.
 * 2. Register a packet handler callback on the packet_handler with
 *    AddStaticPacketType. Typically done in Networks's SetupPacketHandler.
 *    Remember to register both server and client side.
 * 3. Make sure to send the new packet from somewhere! You may want to make
 *    a packet factory function to make it easier to send these types of
 *    packets, see chat.hpp for an example.
 */
enum class PacketHeaderStaticTypes : std::size_t
{
  /**
   * Server syncs its packet types with client, make sure they agree on
   * (name - key) combination.
   */
  kSync = 0,

  /**
   * When a player joins the server, it will send a join request introducing
   * itself and its character.
   */
  kPlayerJoin,

  /**
   * When a player leaves, the rest of the players will get notified with
   * this packet.
   */
  kPlayerLeave,

  /**
   * When a player updates it own state, they send a PlayerUpdate. If the
   * server accepts the PlayerUpdate, it will forward the packet to all
   * other active players.
   */
  kPlayerUpdate,

  /**
   * When the server rejects a PlayerUpdate, it will respond with this
   * packet type.
   */
  kPlayerUpdateRejected,

  /**
   * Contains the players last MoveableIncrement.
   */
  kPlayerIncrement,

  /**
   * Holds the players current player input.
   */
  kPlayerInput,

  /**
   * An item was created, or its state was changed.
   */
  kItemUpdate,

  /**
   * A npc was created, or its state was changed.
   */
  kNpcUpdate,

  /**
   * A projectile was created, or its state was changed.
   */
  kProjectileUpdate,

  /**
   * A tile was created, or its state was changed.
   */
  kTileUpdate,

  // ============================================================ //
  // Must be last, used to count number of elements in the enum
  /**
   * Chat message packets.
   */
  kChat
};

constexpr std::size_t kPacketHeaderStaticTypesCount =
  static_cast<std::size_t>(PacketHeaderStaticTypes::kChat) + 1;

// ============================================================ //
// Helper Types
// ============================================================ //

using PacketHandlerCallback = std::function<void(const Packet&)>;

/**
 * Describes a packet type and how to handle it.
 * PacketHeaderType is stored separately, as key in a map.
 */
struct PacketTypeMeta
{
  // stored as key in map
  /* PacketHeaderType type; */

  // unique name
  String name;

  // called when this packet type is received.
  PacketHandlerCallback callback;
};

/**
 * Like PacketTypeMeta but without callback, used when serializing the data.
 */
struct PacketTypeMetaSerializable
{
  PacketHeaderType type;
  String name;

  bool ToBytes(alflib::RawMemoryWriter& mr) const
  {
    mr.Write(type);
    return mr.Write(name);
  }

  static PacketTypeMetaSerializable FromBytes(alflib::RawMemoryReader& mr)
  {
    PacketTypeMetaSerializable p{};
    p.type = mr.Read<decltype(type)>();
    p.name = mr.Read<decltype(name)>();
    return p;
  }
};

// ============================================================ //
// Packet Handler
// ============================================================ //

/**
 * Packet handler maps a packet type with a callback function.
 */
class PacketHandler
{
public:
  PacketHandler();
  ~PacketHandler();

  // ============================================================ //
  // Packet Consumer
  // ============================================================ //
public:
  /**
   * @return If the packet type was known.
   */
  bool HandlePacket(const Packet& packet) const;

  /**
   *
   * @param packet_type_name Must be unique.
   * @return If name is not unique, the add fails and returns false
   */
  bool AddDynamicPacketType(const String& packet_type_name,
                            PacketHandlerCallback callback);

  /**
   *
   * @param packet_type_name Must be unique.
   * @return If name is not unique, the add fails and returns false
   */
  bool AddStaticPacketType(const PacketHeaderStaticTypes static_type,
                           const String& packet_type_name,
                           PacketHandlerCallback callback);

  enum class SyncResult
  {
    kSuccess = 0,

    // we have fewer packet type's than other
    kMissingPacketType,

    // we have more packet type's than other
    kExtraPacketType,

    // some packet_meta's have same hash, but different names.
    kNameMissmatch
  };

  String SyncResultToString(const SyncResult result);

  /**
   * From a correct set of PacketType's, check our PacketTypes's and
   * correct them if possible.
   *
   * Correcting a PacketType means that the same PacketType name in our
   * list of PacketTypes, must have thier id match the one in correct's
   * list of PacketTypes.
   *
   * Example
   *   Before Sync:
   *     Our     PlayerTypes = ["chat": 10, "player update": 20]
   *     Correct PlayerTypes = ["chat": 12, "player update": 22]
   *
   *   After Sync:
   *     Our     PlayerTypes = ["chat": 12, "player update": 22]
   *     Correct PlayerTypes = ["chat": 12, "player update": 22]
   */
  SyncResult Sync(const std::vector<PacketTypeMetaSerializable>& correct);

  // ============================================================ //

  // used in test, never use this yourself
  bool UnsafeAddDynamicPacketType(const String& packet_type_name,
                                  const PacketHeaderType type_hint,
                                  PacketHandlerCallback callback);

private:
  bool AddDynamicPacketTypeBase(const String& packet_type_name,
                                PacketHeaderType type_hint,
                                PacketHandlerCallback callback);

  // ============================================================ //
  // Packet Producer
  // ============================================================ //
public:
  /**
   * Make a header of static packet type, and fill it in for the packet.
   */
  void BuildPacketHeader(Packet& packet,
                         const PacketHeaderStaticTypes static_type) const;

  /**
   * Make a header of dynamic packet type, and fill it in for the packet.
   * @return If we can find a packet type for the given name.
   */
  bool BuildPacketHeader(Packet& packet, const String& name) const;

private:
  /**
   * Prepare the packet header.
   */
  void BuildPacketHeader(Packet& packet, const PacketHeaderType type) const;

public:
  void BuildPacketSync(Packet& packet);

  /**
   * When you get a sync packet, call this and it will sync for you.
   */
  void OnPacketSync(const Packet& packet);

  // ============================================================ //
  // Misc
  // ============================================================ //
public:
  /**
   * Serialize all our packet_type_meta's. The output from this can be used
   * as input in Sync.
   */
  std::vector<PacketTypeMetaSerializable> Serialize() const;

  /**
   * Search for the packet type with the given name, among the packet types
   * that are dynamic.
   */
  std::optional<PacketHeaderType> FindDynamicType(const String& name) const;

  auto begin() { return packet_type_metas_.begin(); }

  auto end() { return packet_type_metas_.end(); }

  std::size_t GetSize() const { return packet_type_metas_.size(); }

  PacketTypeMeta& operator[](const PacketHeaderType& i)
  {
    return packet_type_metas_[i];
  }

  std::optional<const String*> GetPacketType(const Packet& packet) const;

  /**
   * Print all our packet types.
   */
  void PrintPacketTypes() const;

  /**
   * Return a string representation of our packet types.
   */
  std::string PacketTypesToString() const;

  // ============================================================ //
  // Member Variables
  // ============================================================ //

private:
  std::unordered_map<PacketHeaderType, PacketTypeMeta> packet_type_metas_;

  /**
   * The PacketHeaderStaticTypes points to an index in this array.
   */
  std::array<PacketHeaderType, kPacketHeaderStaticTypesCount> static_types_{};

  std::unordered_map<String, PacketHeaderType> dynamic_types_{};
};
}

#endif // PACKET_HANDLER_HPP_
