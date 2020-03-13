#pragma once

#include "shared/math/math.hpp"
#include "shared/types.hpp"
#include <vector>

namespace wind {
class WindSystem {
public:

  static constexpr u64 kWindVolumeLayer = 1024;

  enum class VolumeType {
    kCube = 0,
    kCylinder,
  };

  static String volumeTypeToString(VolumeType type);
  static VolumeType stringToVolumeType(const String& type);
  static u8 volumeTypeToU8(VolumeType type);
  static VolumeType u8ToVolumeType(u8 type);

  /**
   * @pre Make sure wind volume is in layer kWindVolumeLayer
   */
  void addWindVolume(bs::HSceneObject windVolume);

  void removeWindVolume(bs::HSceneObject windVolume);

  Vec3F getWindAtPoint(Vec3F point) const;



private:
  std::vector<bs::HSceneObject> m_windVolumes;
};
} // namespace wind
