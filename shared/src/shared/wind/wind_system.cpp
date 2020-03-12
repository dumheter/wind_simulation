#include "wind_system.hpp"
#include "shared/log.hpp"

namespace wind {

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
