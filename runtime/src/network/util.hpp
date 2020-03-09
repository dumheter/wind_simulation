#pragma once

#include "shared/scene/component_data.hpp"
#include "shared/scene/types.hpp"
#include "shared/state/moveable_state.hpp"
#include <BsCorePrerequisites.h>
#include <vector>

namespace wind {

class Packet;

struct RequestCreateInfo {
  ObjectType type;
  UniqueId parent;
  bs::Vector3 scale;
  MoveableState state;
  std::vector<ComponentData> components;
};

using CreateInfo = RequestCreateInfo;

namespace PacketBuilder {

void RequestCreate(Packet &packet, const RequestCreateInfo &info);

void Create(Packet &packet, const CreateInfo &info);

} // namespace PacketBuilder

namespace PacketParser {

RequestCreateInfo RequestCreate(const Packet &packet);

CreateInfo Create(const Packet &packet);

} // namespace PacketParser

} // namespace wind
