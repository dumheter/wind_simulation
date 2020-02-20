#pragma once

#include "BsApplication.h"
#include "BsFPSCamera.h"
#include "GUI/BsGUILabel.h"
#include "app.hpp"
#include "cmyplayer.hpp"
#include "cnet_component.hpp"
#include "creator.hpp"
#include "crotor.hpp"
#include "network/connection_id.hpp"
#include "network/server.hpp"
#include "player_input.hpp"
#include "utility/unique_id.hpp"
#include <GUI/BsGUITexture.h>
#include <unordered_map>

namespace wind {

class NetDebugInfo {
public:
  void setup(bs::GUILayoutY *layout);

  void update(const Client &client);

private:
  bs::GUILabel *hCQL;
  bs::GUILabel *hCQR;
  bs::GUILabel *hBSCE;
  bs::GUILabel *hping;
  bs::GUILabel *houtBytes;
  bs::GUILabel *houtPackets;
  bs::GUILabel *hinBytes;
  bs::GUILabel *hinPackets;
  bs::GUILabel *hqueueTime;

  bs::String CQL;
  bs::String CQR;
  bs::String BSCE;
  bs::String ping;
  bs::String outBytes;
  bs::String outPackets;
  bs::String inBytes;
  bs::String inPackets;
  bs::String queueTime;
};

class World : public App {
public:
  World(const App::Info &info);

  /**
   * Reset the world state. (Delete players etc.)
   */
  void reset();

  void onPreUpdate(f32) override;

  void onFixedUpdate(f32) override;

  void onWindowResize() override;

  /**
   * Add a player to our world.
   * @param connection An active connection to a client.
   */
  void onPlayerJoin(const MoveableState &moveableState);

  void onPlayerLeave(UniqueId uid);

  void onPlayerInput(UniqueId uid, PlayerInput input,
                     std::optional<bs::Quaternion> maybeRot);

  void onDisconnect();

  /**
   * Build the player entity, camera and attach gui to the
   * camera.
   * @pre m_players must be empty
   */
  void setupMyPlayer();

  void applyMoveableState(const MoveableState &moveableState);

  /**
   * We only apply the moveable state to our player if we have diverged too
   * much.
   * @pre moveableState should represent myplayer's moveable state.
   */
  void applyMyMoveableState(const MoveableState &moveableState);

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

  const std::unordered_map<UniqueId, HCNetComponent> &getNetComps() const {
    return m_netComps;
  }

  std::unordered_map<UniqueId, bs::HFPSWalker> &getWalkers() {
    return m_walkers;
  }

  const std::unordered_map<UniqueId, bs::HFPSWalker> &getWalkers() const {
    return m_walkers;
  }

  const Creator &getCreator() const { return m_creator; }

  const bs::HFPSCamera &getFpsCamera() const { return m_fpsCamera; }

private:
  void setupInput();

  bs::HSceneObject createCamera(bs::HSceneObject player);

  bs::HSceneObject createFloor(bs::HMaterial material,
                               bs::HPhysicsMaterial physicsMaterial);
  bs::HPhysicsMaterial createPhysicsMaterial();
  bs::HMaterial createMaterial(const bs::String &path);
  bs::HSceneObject createGUI(bs::HSceneObject camera);

  /* Give full window width and height. */
  void updateAimPosition(u32 width, u32 height);

private:
  std::unordered_map<UniqueId, HCNetComponent> m_netComps;
  std::unordered_map<UniqueId, bs::HFPSWalker> m_walkers;
  bool m_cursorMode = false;
  bs::HFPSCamera m_fpsCamera;
  Server m_server;
  HCMyPlayer m_player;
  Creator m_creator;
  NetDebugInfo m_netDebugInfo{};
  bs::GUITexture *m_aim;
};

} // namespace wind
