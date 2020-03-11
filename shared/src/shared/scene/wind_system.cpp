#include "wind_system.hpp"

namespace wind {

void WindSystem::addWindVolume(bs::HSceneObject windVolume) {
  m_windVolumes.push_back(windVolume);
}

void WindSystem::removeWindVolume(bs::HSceneObject windVolume) {
  //TODO uniquely identify a volume, use unique id?
}
  
Vec3F WindSystem::getWindAtPoint(Vec3F point) const {
  Vec3F force = Vec3F::ZERO;
  for (const auto& windVolume : m_windVolumes) {
    wind += windVolume->getWindAtPoint(point);
  }
  return force;
}

}
