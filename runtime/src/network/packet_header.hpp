#ifndef PACKET_HEADER_HPP_
#define PACKET_HEADER_HPP_

#include "common.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

namespace wind {

using PacketHeaderTypesUnderlying = u32;
enum class PacketHeaderTypes : PacketHeaderTypesUnderlying {
  kPlayerJoin = 0,
  kPlayerLeave,
  kPlayerUpdate,
  kHello,
};

struct PacketHeader {
  PacketHeaderTypes type;

  bool ToBytes(alflib::RawMemoryWriter &mw) const {
    return mw.Write(static_cast<PacketHeaderTypesUnderlying>(type));
  }

  static PacketHeader FromBytes(alflib::RawMemoryReader &mr) {
    PacketHeader p{};
    p.type =
        static_cast<decltype(type)>(mr.Read<PacketHeaderTypesUnderlying>());
    return p;
  }
};

} // namespace wind

#endif // PACKET_HEADER_HPP_
