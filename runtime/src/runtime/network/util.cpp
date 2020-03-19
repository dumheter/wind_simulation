#include "util.hpp"
#include "runtime/network/packet.hpp"
#include "shared/state/moveable_state.hpp"

namespace wind {

namespace PacketBuilder {

void RequestCreate(Packet &packet, const RequestCreateInfo &info) {
  packet.ClearPayload();
  packet.SetHeader(PacketHeaderTypes::kRequestCreate);
  auto mw = packet.GetMemoryWriter();
  mw->Write(static_cast<std::underlying_type<ObjectType>::type>(info.type));
  mw->Write(info.parent);
  mw->Write(info.scale.x);
  mw->Write(info.scale.y);
  mw->Write(info.scale.z);
  mw->Write(info.state);
  mw->Write(static_cast<u32>(info.components.size()));
  for (auto &component : info.components) {
    mw->Write(component);
  }
  mw.Finalize();
}

void Create(Packet &packet, const CreateInfo &info) {
  packet.ClearPayload();
  packet.SetHeader(PacketHeaderTypes::kCreate);
  auto mw = packet.GetMemoryWriter();
  mw->Write(static_cast<std::underlying_type<ObjectType>::type>(info.type));
  mw->Write(info.parent);
  mw->Write(info.scale.x);
  mw->Write(info.scale.y);
  mw->Write(info.scale.z);
  mw->Write(info.state);
  mw->Write(static_cast<u32>(info.components.size()));
  for (auto &component : info.components) {
    mw->Write(component);
  }
  mw.Finalize();
}

} // namespace PacketBuilder

namespace PacketParser {

RequestCreateInfo RequestCreate(const Packet &packet) {
  auto mr = packet.GetMemoryReader();
  RequestCreateInfo info{};
  info.type = static_cast<ObjectType>(
      mr.Read<std::underlying_type<ObjectType>::type>());
  info.parent = mr.Read<UniqueId>();
  info.scale = bs::Vector3{mr.Read<f32>(), mr.Read<f32>(), mr.Read<f32>()};
  info.state = mr.Read<MoveableState>();
  const auto compCount = mr.Read<u32>();
  info.components.reserve(compCount);
  for (u32 i = 0; i < compCount; ++i) {
    info.components.push_back(mr.Read<ComponentData>());
  }
  return info;
}

CreateInfo Create(const Packet &packet) {
  auto mr = packet.GetMemoryReader();
  RequestCreateInfo info{};
  info.type = static_cast<ObjectType>(
      mr.Read<std::underlying_type<ObjectType>::type>());
  info.parent = mr.Read<UniqueId>();
  info.scale = bs::Vector3{mr.Read<f32>(), mr.Read<f32>(), mr.Read<f32>()};
  info.state = mr.Read<MoveableState>();
  const auto compCount = mr.Read<u32>();
  info.components.reserve(compCount);
  for (u32 i = 0; i < compCount; ++i) {
    info.components.push_back(mr.Read<ComponentData>());
  }
  return info;
}

} // namespace PacketParser

} // namespace wind
