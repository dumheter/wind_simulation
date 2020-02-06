#ifndef PACKET_HEADER_HPP_
#define PACKET_HEADER_HPP_

#include "common.hpp"

namespace wind {

using PacketHeaderType = u32;

struct PacketHeader
{
  PacketHeaderType type;
};

}

#endif // PACKET_HEADER_HPP_
