#pragma once

#include "BsApplication.h"
#include "app.hpp"
#include <vector>

namespace wind {

class World : public App {
public:
  World(const App::Info &info);

  /**
   * Reset the world state. (Delete players etc.)
   */
  void reset();

  /**
   * Build the player entity, camera and attach gui to the
   * camera.
   * @pre m_players must be empty
   */
  void setupPlayer();

  /**
   * Load the default scene.
   */
  void setupScene();

  bs::HSceneObject getPlayer();

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

  std::vector<bs::HSceneObject> m_players;
};

} // namespace wind
