#include "wind_system.hpp"
#include "shared/log.hpp"

namespace wind {

String WindSystem::volumeTypeToString(VolumeType type) {
  switch (type) {
  case VolumeType::kCylinder: {
    return "cylinder";
  }
  case VolumeType::kCube:
    [[fallthrough]];
  default:
    return "cube";
  }
}

WindSystem::VolumeType WindSystem::stringToVolumeType(const String &type) {
  if (type == "cylinder") {
    return VolumeType::kCylinder;
  }
  return VolumeType::kCube;
}

u8 WindSystem::volumeTypeToU8(VolumeType type) {
  return static_cast<u8>(type);
}

WindSystem::VolumeType WindSystem::u8ToVolumeType(u8 type) {
  return static_cast<VolumeType>(type);
}

void WindSystem::addWindVolume(bs::HSceneObject windVolume) {
  m_windVolumes.push_back(windVolume);
}

void WindSystem::removeWindVolume(bs::HSceneObject windVolume) {
  // TODO uniquely identify a volume, use unique id?
  logError("[windSystem] remove TODO");
}

Vec3F WindSystem::getWindAtPoint(Vec3F point) const {
  Vec3F wind = Vec3F::ZERO;
  for (const auto &windVolume : m_windVolumes) {
    // TODO
    // wind += windVolume->getWindAtPoint(point);
  }
  return wind;
}

} // namespace wind
