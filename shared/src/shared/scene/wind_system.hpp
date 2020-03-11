#pragma once

#include "shared/math/math.hpp"
#include <vector>

namespace wind
{
class WindSystem
{
public:

  void addWindVolume(bs::HSceneObject windVolume);

  void removeWindVolume(bs::HSceneObject windVolume);
  
  Vec3F getWindAtPoint(Vec3F point) const;

private:
  std::vector<bs::HSceneObject> m_windVolumes;
};
}
