#pragma once

#include "app.hpp"
#include "BsApplication.h"

namespace wind {


class World : public App
{
 public:

  World(const App::Info& info);

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
};


}
