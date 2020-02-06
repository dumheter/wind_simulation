#pragma once

#include "BsApplication.h"
#include "app.hpp"

namespace wind {

class World : public App {
public:
  World(const App::Info &info);

private:
  void setupInput();

  bs::HSceneObject createCamera(bs::HSceneObject player);
  bs::HSceneObject createPlayer();
  bs::HSceneObject createCube(bs::HMaterial material,
                              bs::HPhysicsMaterial physicsMaterial);
  bs::HSceneObject createFloor(bs::HMaterial material,
                               bs::HPhysicsMaterial physicsMaterial);
  bs::HPhysicsMaterial createPhysicsMaterial();
  bs::HMaterial createMaterial(const bs::String &path);
  bs::HSceneObject createGUI(bs::HSceneObject camera);

  // Network<kServer> m_server;
  // Network<kClient> m_client;
};

} // namespace wind
