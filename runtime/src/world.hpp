#pragma once

#include "BsApplication.h"
#include "BsFPSCamera.h"
#include "app.hpp"
#include "cmyplayer.hpp"
#include "cnet_component.hpp"
#include "creator.hpp"
#include "network/connection_id.hpp"
#include "network/server.hpp"
#include "utility/unique_id.hpp"
#include <unordered_map>

namespace wind {

class World : public App {
public:
  World(const App::Info &info);

  /**
   * Reset the world state. (Delete players etc.)
   */
  void reset();

  void onPreUpdate() override;

  void onFixedUpdate() override;

  /**
   * Add a player to our world.
   * @param connection An active connection to a client.
   */
  void onPlayerJoin(const MoveableState &moveableState);

  void onPlayerLeave(UniqueId uid);

  /**
   * Build the player entity, camera and attach gui to the
   * camera.
   * @pre m_players must be empty
   */
  void setupMyPlayer();

  void applyMoveableState(const MoveableState &moveableState);

  /**
   * Load the default scene.
   */
  void setupScene();

  HCNetComponent getPlayerNetComp();

  void addNetComp(HCNetComponent netComp);

  void removeNetComp(HCNetComponent netComp);

  /**
   * @return If it could be changed successfully.
   */
  bool netCompChangeUniqueId(UniqueId from, UniqueId to);

  bool serverIsActive() const { return m_server.isActive(); }

  std::unordered_map<UniqueId, HCNetComponent> &getNetComps() {
    return m_netComps;
  }

  const Creator& getCreator() { return m_creator; }

private:
  void setupInput();

  bs::HSceneObject createCamera(bs::HSceneObject player);

  bs::HSceneObject createFloor(bs::HMaterial material,
                               bs::HPhysicsMaterial physicsMaterial);
  bs::HPhysicsMaterial createPhysicsMaterial();
  bs::HMaterial createMaterial(const bs::String &path);
  bs::HSceneObject createGUI(bs::HSceneObject camera);

private:
  std::unordered_map<UniqueId, HCNetComponent> m_netComps;
  bool m_cursorMode = true;
  bs::HFPSCamera m_fpsCamera;
  Server m_server;
  HCMyPlayer m_player;
  Creator m_creator;
};

} // namespace wind
