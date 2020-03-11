#pragma once

#include "cmyplayer.hpp"
#include "network/connection_id.hpp"
#include "network/server.hpp"
#include "network/util.hpp"
#include "shared/app.hpp"
#include "shared/scene/cnet_component.hpp"
#include "shared/scene/crotor.hpp"
#include "shared/scene/fps_camera.hpp"
#include "shared/state/player_input.hpp"
#include "shared/utility/unique_id.hpp"
#include <GUI/BsGUILabel.h>
#include <GUI/BsGUITexture.h>
#include <unordered_map>

namespace wind {

class NetDebugInfo {
public:
  void setup(bs::GUILayoutY *layout);

  void update(const Client &client);

private:
  bs::GUILabel *hCQL{};
  bs::GUILabel *hCQR{};
  bs::GUILabel *hBSCE{};
  bs::GUILabel *hping{};
  bs::GUILabel *houtBytes{};
  bs::GUILabel *houtPackets{};
  bs::GUILabel *hinBytes{};
  bs::GUILabel *hinPackets{};
  bs::GUILabel *hqueueTime{};

  bs::String CQL{};
  bs::String CQR{};
  bs::String BSCE{};
  bs::String ping{};
  bs::String outBytes{};
  bs::String outPackets{};
  bs::String inBytes{};
  bs::String inPackets{};
  bs::String queueTime{};
};

// ============================================================ //

class World : public App {
public:
  explicit World(const App::Info &info);

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

  void buildObject(const CreateInfo &info);

  /**
   * Build the player entity, camera and attach gui to the
   * camera.
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
   * Use this if there are untracked netcomponents,
   * (that the world doesnt know about).
   */
  void scanForNetComps();

  void dumpNetComps();

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

  const HFPSCamera &getFpsCamera() const { return m_fpsCamera; }

  bool getCursorMode() const { return m_cursorMode; }

  const String &getScenePath() const { return m_scenePath; }

  void setStaticScene(bs::HSceneObject so) { m_staticScene = so; }
  bs::HSceneObject getStaticScene() const { return m_staticScene; }

  void setDynamicScene(bs::HSceneObject so) { m_dynamicScene = so; }
  bs::HSceneObject getDynamicScene() const { return m_dynamicScene; }

private:
  void setupInput();

  bs::HSceneObject createCamera(bs::HSceneObject player);

  bs::HSceneObject createFloor(bs::HMaterial material,
                               bs::HPhysicsMaterial physicsMaterial);
  bs::HPhysicsMaterial createPhysicsMaterial();
  bs::HMaterial createMaterial(const bs::String &path);
  bs::HSceneObject createGUI(bs::HSceneObject camera);

  /// Give full window width and height.
  void updateAimPosition(u32 width, u32 height);

private:
  std::unordered_map<UniqueId, HCNetComponent> m_netComps;
  bool m_cursorMode = false;
  HFPSCamera m_fpsCamera;
  Server m_server;
  HCMyPlayer m_player;
  NetDebugInfo m_netDebugInfo{};
  bs::GUITexture *m_aim;
  String m_scenePath{"res/scenes/wind.json"};
  bs::HSceneObject m_staticScene;
  bs::HSceneObject m_dynamicScene;
  bs::GUISliderHorz *m_shootForce;
  bs::VirtualAxis m_scroll;
};

} // namespace wind
