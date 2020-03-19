#pragma once

#include "shared/types.hpp"

namespace wind
{

class Client;
class World;

// ============================================================ //

class NetDebugInfo {
 public:
  NetDebugInfo();

  ~NetDebugInfo();

  void setup(bs::GUILayoutY *layout);

  void update(const Client &client);

 private:
  struct Data;
  Data *m_data;
};

// ============================================================ //

class Ui {
 public:
  Ui() = default;

  void setup(World *world, bs::HSceneObject camera, u32 width, u32 height);

  /// Give full window width and height.
  void updateAimPosition(u32 width, u32 height);

  f32 getShootForce() const;

  void setShootForce(f32 percent);

  NetDebugInfo &getNetDebugInfo() { return m_netDebugInfo; }

 private:
  bs::GUITexture *m_aim{};
  bs::GUISliderHorz *m_shootForce{};
  NetDebugInfo m_netDebugInfo{};
};

}
