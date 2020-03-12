#pragma once

#include "shared/math/math.hpp"
#include <vector>

namespace wind {
class WindSystem {
public:

  /**
   * @pre Make sure wind volume is in layer kWindVolumeLayer
   */
  void addWindVolume(bs::HSceneObject windVolume);

  void removeWindVolume(bs::HSceneObject windVolume);

  Vec3F getWindAtPoint(Vec3F point) const;

  static constexpr u64 kWindVolumeLayer = 1024;

private:
  std::vector<bs::HSceneObject> m_windVolumes;
};
} // namespace wind
