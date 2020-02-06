#include "packet_handler.hpp"
#include "core/hash.hpp"
#include <alflib/core/assert.hpp>
#include <dlog.hpp>
#include <microprofile/microprofile.h>

namespace dib {

PacketHandler::PacketHandler() = default;

PacketHandler::~PacketHandler() = default;

bool
PacketHandler::HandlePacket(const Packet& packet) const
{
  MICROPROFILE_SCOPEI("packet handler", "handle packet", MP_YELLOW);
  bool could_handle = false;

  const PacketHeader* header = packet.GetHeader();
  auto it = packet_type_metas_.find(header->type);
  if (it != packet_type_metas_.end()) {
    could_handle = true;
    // TODO maybe not send the header, or clear the header first???
    it->second.callback(packet);
  } else {
    DLOG_WARNING("got packet of unknown type [{}], ignoring", header->type);
  }

  return could_handle;
}

bool
PacketHandler::AddDynamicPacketType(const String& packet_type_name,
                                    PacketHandlerCallback callback)
{
  PacketHeaderType type_hint =
    HashFNV1a32(packet_type_name.GetUTF8(), packet_type_name.GetSize());

  return AddDynamicPacketTypeBase(packet_type_name, type_hint, callback);
}

bool
PacketHandler::UnsafeAddDynamicPacketType(const String& packet_type_name,
                                          const PacketHeaderType type_hint,
                                          PacketHandlerCallback callback)
{
  return AddDynamicPacketTypeBase(packet_type_name, type_hint, callback);
}

bool
PacketHandler::AddDynamicPacketTypeBase(const String& packet_type_name,
                                        PacketHeaderType type_hint,
                                        PacketHandlerCallback callback)
{
  // check for name collision
  for (const auto it : packet_type_metas_) {
    if (it.second.name == packet_type_name) {
      DLOG_ERROR("could not add dynamic packet type for [{}] since it "
                 "already exist an entry with that name.",
                 packet_type_name);
      return false;
    }
  }

  // try @tries times to come up with a unique hash for the packet type
  int tries = 50;
  while (tries-- > 0) {
    // ensure no collision
    for (;;) {
      if (packet_type_metas_.find(type_hint) == packet_type_metas_.end()) {
        break;
      }
      ++type_hint;
    }

    [[maybe_unused]] const auto [it, ok] =
      packet_type_metas_.insert({ type_hint, { packet_type_name, callback } });
    if (ok) {
      // add it to our name-type map
      dynamic_types_.insert({ packet_type_name, it->first });
      break;
    } else {
      ++type_hint;
    }
  }

  AlfAssert(tries != 0,
            "could not add packet type [{}] after 50 attempts",
            packet_type_name);

  return true;
}

bool
PacketHandler::AddStaticPacketType(const PacketHeaderStaticTypes static_type,
                                   const String& packet_type_name,
                                   PacketHandlerCallback callback)
{
  AlfAssert(static_cast<std::size_t>(static_type) >= 0 &&
              static_cast<std::size_t>(static_type) <
                kPacketHeaderStaticTypesCount,
            "unknown static_type");
  auto size = sizeof(static_type);
  PacketHeaderType type_hint =
    HashFNV1a32(reinterpret_cast<const u8*>(&static_type), size);

  // Check for name collision
  for (const auto it : packet_type_metas_) {
    if (it.second.name == packet_type_name) {
      DLOG_ERROR("could not add static packet type for [{}] since it already "
                 "exists an entry with that name.",
                 packet_type_name);
      return false;
    }
  }

  // try @tries times to come up with a unique hash for the packet type
  int tries = 50;
  while (tries-- > 0) {
    // ensure no collision
    for (;;) {
      if (packet_type_metas_.find(type_hint) == packet_type_metas_.end()) {
        break;
      }
      ++type_hint;
    }

    [[maybe_unused]] const auto [it, ok] =
      packet_type_metas_.insert({ type_hint, { packet_type_name, callback } });
    if (ok) {
      static_types_[static_cast<std::size_t>(static_type)] = it->first;
      break;
    } else {
      ++type_hint;
    }
  }

  AlfAssert(tries != 0,
            "could not add packet type [{}] after 50 attempts",
            packet_type_name);

  return true;
}

PacketHandler::SyncResult
PacketHandler::Sync(const std::vector<PacketTypeMetaSerializable>& correct)
{
  // are we missing a packet meta?
  if (packet_type_metas_.size() < correct.size()) {
    DLOG_ERROR("we are missing {} packet types",
               correct.size() - packet_type_metas_.size());
    return SyncResult::kMissingPacketType;
  }

  // do we have extra packet meta?
  if (packet_type_metas_.size() > correct.size()) {
    DLOG_VERBOSE("we have {} more packet types than our sync source",
                 packet_type_metas_.size() - correct.size());
  }

  // for each name, are the hashes correct?
  String missing_names{};
  std::vector<PacketTypeMetaSerializable> missing_typees{};
  bool found_name;
  bool found_type;
  for (const auto& correct_meta : correct) {
    found_name = false;
    found_type = false;
    for (const auto& meta : packet_type_metas_) {
      if (correct_meta.name == meta.second.name) {
        found_name = true;
        if (correct_meta.type == meta.first) {
          found_type = true;
          break;
        }
      }
    }
    if (!found_type) {
      missing_typees.push_back(correct_meta);
    }
    if (!found_name) {
      if (missing_names.GetSize() != 0) {
        missing_names += ", ";
      }
      missing_names += correct_meta.name;
    }
  }
  if (missing_names.GetSize() != 0) {
    DLOG_ERROR("unable to find these packet types [{}]", missing_names);
    return SyncResult::kNameMissmatch;
  }

  // correct the type-name
  std::vector<std::pair<PacketHeaderType, PacketTypeMeta>> insert_vec{};
  for (const auto& missing_type : missing_typees) {

    // is it a dynamic type?
    if (auto dyn_it = dynamic_types_.find(missing_type.name);
        dyn_it != dynamic_types_.end()) {
      auto packet_type_metas_it = packet_type_metas_.find(dyn_it->second);
      AlfAssert(packet_type_metas_it != packet_type_metas_.end(),
                "could not find"
                " packet type, but previous code guarantees it");
      insert_vec.push_back(
        { missing_type.type, { packet_type_metas_it->second } });
      packet_type_metas_.erase(packet_type_metas_it);

      dyn_it->second = missing_type.type;
    }

    // else must be a static type
    else {
      std::size_t i = 0;
      bool found = false;
      for (; i < static_types_.size(); i++) {
        if (packet_type_metas_[static_types_[i]].name == missing_type.name) {
          found = true;
          break;
        }
      }
      AlfAssert(found,
                "could not find static packet type, but previous code"
                " guarantees it.");

      auto packet_type_metas_it = packet_type_metas_.find(static_types_[i]);
      AlfAssert(packet_type_metas_it != packet_type_metas_.end(),
                "could not find"
                " packet type, but previous code guarantees it");

      insert_vec.push_back(
        { missing_type.type, { packet_type_metas_it->second } });
      packet_type_metas_.erase(packet_type_metas_it);

      static_types_[i] = missing_type.type;
    }
  }
  if (!insert_vec.empty()) {
    for (const auto& item : insert_vec) {
      packet_type_metas_.insert(item);
    }
  }

  return SyncResult::kSuccess;
}

String
PacketHandler::SyncResultToString(const SyncResult result)
{
  switch (result) {
    case SyncResult::kSuccess:
      return "success";
    case SyncResult::kNameMissmatch:
      return "name missmatch";
    case SyncResult::kExtraPacketType:
      return "extra packet type";
    case SyncResult::kMissingPacketType:
      return "missing packet type";
    default:
      return "internal error";
  }
}

void
PacketHandler::BuildPacketHeader(
  Packet& packet,
  const PacketHeaderStaticTypes static_type) const
{
  AlfAssert(static_cast<std::size_t>(static_type) >= 0 &&
              static_cast<std::size_t>(static_type) < static_types_.size(),
            "trying to access index that is out of range");
  BuildPacketHeader(packet,
                    static_types_[static_cast<std::size_t>(static_type)]);
}

bool
PacketHandler::BuildPacketHeader(Packet& packet, const String& name) const
{
  const auto maybe_type = FindDynamicType(name);
  if (maybe_type) {
    BuildPacketHeader(packet, *maybe_type);
    return true;
  }
  return false;
}

void
PacketHandler::BuildPacketHeader(Packet& packet,
                                 const PacketHeaderType type) const
{
  PacketHeader header{};
  header.type = type;
  packet.SetHeader(header);
}

void
PacketHandler::BuildPacketSync(Packet& packet)
{
  BuildPacketHeader(packet, PacketHeaderStaticTypes::kSync);
  auto vec = Serialize();

  auto mw = packet.GetMemoryWriter();
  for (const auto& data : vec) {
    mw->Write(data);
  }
  mw.Finalize();
}

void
PacketHandler::OnPacketSync(const Packet& packet)
{
  std::vector<PacketTypeMetaSerializable> vec{};
  auto mr = packet.GetMemoryReader();

  for (std::size_t pos = 0; pos < packet.GetPayloadSize();
       pos = mr.GetOffset()) {
    vec.push_back(mr.Read<PacketTypeMetaSerializable>());
  }

  const auto res = Sync(vec);
  if (res != SyncResult::kSuccess) {
    DLOG_ERROR("failed to sync due to {}", SyncResultToString(res));
  }
  DLOG_VERBOSE("Sync ok");
}

std::vector<PacketTypeMetaSerializable>
PacketHandler::Serialize() const
{
  std::vector<PacketTypeMetaSerializable> v{};
  for (const auto it : packet_type_metas_) {
    v.push_back({ it.first, it.second.name });
  }
  return v;
}

std::optional<PacketHeaderType>
PacketHandler::FindDynamicType(const String& name) const
{
  if (auto it = dynamic_types_.find(name); it != dynamic_types_.end()) {
    return std::optional<PacketHeaderType>{ it->second };
  } else {
    return std::nullopt;
  }
}

std::optional<const String*>
PacketHandler::GetPacketType(const Packet& packet) const
{
  if (const auto it = packet_type_metas_.find(packet.GetHeader()->type);
      it != packet_type_metas_.end()) {
    return &it->second.name;
  }
  return std::nullopt;
}

void
PacketHandler::PrintPacketTypes() const
{
  DLOG_RAW("\n*** Registered packet types, {}.\n", packet_type_metas_.size());
  DLOG_RAW("{:<15}{}\n", "ID", "PACKET TYPE NAME");
  for (const auto packet_type_meta : packet_type_metas_) {
    DLOG_RAW(
      "#{:<14}{}\n", packet_type_meta.first, packet_type_meta.second.name);
  }
  DLOG_RAW("\n");
}

std::string
PacketHandler::PacketTypesToString() const
{
  auto str = dlog::Format("{:-^34}\n", "Registered Packet Types");
  str += dlog::Format("{:<15}{}\n", "ID", "PACKET TYPE NAME");
  for (const auto packet_type_meta : packet_type_metas_) {
    str += dlog::Format(
      "#{:<14}{}\n", packet_type_meta.first, packet_type_meta.second.name);
  }
  return str;
}
}
