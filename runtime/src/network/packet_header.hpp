#ifndef PACKET_HEADER_HPP_
#define PACKET_HEADER_HPP_

#include "types.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

namespace wind {

/**
 * When adding a new packet type:
 *   1. Handle them both in client and server.
 *   2. Write the producer.
 */
using PacketHeaderTypesUnderlying = u32;
enum class PacketHeaderTypes : PacketHeaderTypesUnderlying {
  kPlayerJoin = 0,
  kPlayerLeave,
  kPlayerTick,
  kServerTick,
  kCreate,
  kRequestCreate,
  kLookup,
  kLookupResponse,
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
