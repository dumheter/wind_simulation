#pragma once

#include "BsApplication.h"
#include "BsFPSCamera.h"
#include "app.hpp"
#include "cnet_component.hpp"
#include "network/client.hpp"
#include "network/connection_id.hpp"
#include "network/server.hpp"
#include "utility/unique_id.hpp"
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
   * Add a player to our world.
   * @param connection An active connection to a client.
   */
  void onPlayerJoin(UniqueId playerUid);

  void onPlayerLeave(UniqueId uid);

  /**
   * Build the player entity, camera and attach gui to the
   * camera.
   * @pre m_players must be empty
   */
  bs::HSceneObject setupMyPlayer();

  bs::HSceneObject setupPlayer();

  /**
   * Load the default scene.
   */
  void setupScene();

  bs::HSceneObject getPlayer();

  void addNetComp(HCNetComponent netComp);
  void removeNetComp(HCNetComponent netComp);

private:
  void setupInput();

  bs::HSceneObject createCamera(bs::HSceneObject player);
  bs::HSceneObject createCube(bs::HMaterial material,
                              bs::HPhysicsMaterial physicsMaterial);
  bs::HSceneObject createFloor(bs::HMaterial material,
                               bs::HPhysicsMaterial physicsMaterial);
  bs::HPhysicsMaterial createPhysicsMaterial();
  bs::HMaterial createMaterial(const bs::String &path);
  bs::HSceneObject createGUI(bs::HSceneObject camera);

private:
  std::vector<HCNetComponent> m_netComps;
  bool cursorMode = true;
  bs::HFPSCamera m_fpsCamera;
  Server m_server;
  Client m_client;
};

} // namespace wind
