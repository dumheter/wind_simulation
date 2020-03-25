#pragma once

#include "runtime/cmyplayer.hpp"
#include "runtime/network/connection_id.hpp"
#include "runtime/network/server.hpp"
#include "runtime/network/util.hpp"
#include "runtime/ui.hpp"
#include "shared/app.hpp"
#include "shared/scene/component/cnet_component.hpp"
#include "shared/scene/component/crotor.hpp"
#include "shared/scene/component/fps_camera.hpp"
#include "shared/state/player_input.hpp"
#include "shared/utility/unique_id.hpp"
#include <filesystem>
#include <unordered_map>

namespace wind {

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

  void onSceneChange(const String &scene);

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

  void reloadStaticScene();

  HCNetComponent getPlayerNetComp();

  void addNetComp(HCNetComponent netComp);

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

  Server &getServer() { return m_server; }
  const Server &getServer() const { return m_server; }

  std::unordered_map<UniqueId, HCNetComponent> &getNetComps() {
    return m_netComps;
  }

  const std::unordered_map<UniqueId, HCNetComponent> &getNetComps() const {
    return m_netComps;
  }

  const HFPSCamera &getFpsCamera() const { return m_fpsCamera; }

  void setScenePath(String path) { m_scenePath = path; }
  const String &getScenePath() const { return m_scenePath; }

  void setStaticScene(bs::HSceneObject so) { m_staticScene = so; }
  bs::HSceneObject getStaticScene() const { return m_staticScene; }

  void setDynamicScene(bs::HSceneObject so) { m_dynamicScene = so; }
  bs::HSceneObject getDynamicScene() const { return m_dynamicScene; }

  HCMyPlayer &getMyPlayer() { return m_player; }
  const HCMyPlayer &getMyPlayer() const { return m_player; }

private:
  void setupInput();

  bs::HSceneObject createCamera(bs::HSceneObject player);

private:
  std::unordered_map<UniqueId, HCNetComponent> m_netComps;
  Ui m_ui{};
  Server m_server;
  HCMyPlayer m_player;
  HFPSCamera m_fpsCamera{};
  bool m_cursorMode = false;
  bs::VirtualAxis m_scroll{};

  bs::HSceneObject m_staticScene;
  bs::HSceneObject m_dynamicScene;
  String m_scenePath{"../res/scenes/bake.json"};
  std::filesystem::file_time_type m_sceneEditTime;
};

} // namespace wind
