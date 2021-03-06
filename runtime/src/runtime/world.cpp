﻿#include "world.hpp"
#include "shared/asset.hpp"
#include "shared/math/math.hpp"
#include "shared/scene/builder.hpp"
#include "shared/scene/component/cwind.hpp"
#include "shared/scene/scene.hpp"
#include "shared/utility/util.hpp"

#include <Components/BsCCamera.h>
#include <Components/BsCCharacterController.h>
#include <Importer/BsImporter.h>
#include <Input/BsInput.h>
#include <Math/BsQuaternion.h>
#include <Platform/BsCursor.h>
#include <dlog/dlog.hpp>
#include <microprofile/microprofile.h>
#include <regex>

namespace wind {

World::World(const App::Info &info) : App(info), m_server(this) {
  using namespace bs;

  setupMyPlayer();
  setupInput();
  if (std::filesystem::exists(m_scenePath)) {
    std::filesystem::file_time_type time =
        std::filesystem::last_write_time(m_scenePath);
    m_sceneEditTime = time;
  }
}

void World::onPreUpdate(f32) {
  MICROPROFILE_SCOPEI("world", "onPreUpdate", MP_BLUE2);
  if (m_cursorMode) {
    Util::centerCursor(bs::gApplication().getPrimaryWindow());
  }

  m_server.Poll();

  if (f32 v = bs::gVirtualInput().getAxisValue(m_scroll); std::abs(v) > 0.1f) {
    constexpr f32 kFromExtreme = 512.0f;
    constexpr f32 kToExtreme = 0.1f;
    v = clamp(v, -kFromExtreme, kFromExtreme);
    v = map(v, -kFromExtreme, kFromExtreme, -kToExtreme, kToExtreme);
    const f32 new_percent = m_ui.getShootForce() + v;
    m_ui.setShootForce(new_percent);
    m_player->setShootForce(100.0f * new_percent);
  }
}

void World::onFixedUpdate(f32) {
  using namespace bs;
  MICROPROFILE_SCOPEI("world", "onFixedUpdate", MP_BLUE3);

  if (m_player) {
    m_ui.getNetDebugInfo().update(m_player->getClient());
  }

  static u32 i = 0;

  if (++i > 2) {
    i = 0;
    m_server.broadcastServerTick(m_netComps);
  }

  // Check current file modification date
  if (!m_scenePath.empty()) {
    std::filesystem::path path = m_scenePath.c_str();
    if (std::filesystem::exists(path)) {
      std::filesystem::file_time_type time =
          std::filesystem::last_write_time(path);
      if (time != m_sceneEditTime) {
        reloadStaticScene();
      }
      m_sceneEditTime = time;
    }
  }
}

void World::onWindowResize() { m_ui.updateAimPosition(m_width, m_height); }

void World::setupScene() {
  DLOG_VERBOSE("loading scene");
  // TODO set file path in gui
  m_staticScene = Scene::loadFile(m_scenePath);
  std::filesystem::file_time_type time =
      std::filesystem::last_write_time(m_scenePath);
  m_sceneEditTime = time;

  m_dynamicScene = SceneBuilder{}.withName("dynamicScene").build();
  m_player->SO()->setPosition(bs::Vector3::ZERO);
}

void World::reloadStaticScene() {
  if (m_server.isActive()) {
    m_staticScene->destroy();
    m_staticScene = Scene::loadFile(m_scenePath);

    DLOG_VERBOSE(
        "[world:reloadStaticScene] (server) broadcasting reload scene packet");
    std::filesystem::file_time_type time =
        std::filesystem::last_write_time(m_scenePath);
    m_sceneEditTime = time;

    auto &packet = m_server.getPacket();
    packet.ClearPayload();
    packet.SetHeader(PacketHeaderTypes::kSceneChange);
    auto mw = packet.GetMemoryWriter();
    nlohmann::json json = Scene::saveScene(m_staticScene);
    mw->Write(alflib::String(json.dump().c_str()));
    mw.Finalize();
    m_server.PacketBroadcast(packet, SendStrategy::kReliable);
  }
}

HCNetComponent World::getPlayerNetComp() {
  if (m_player) {
    auto uid = m_player->getUniqueId();
    return m_netComps[uid];
  }
  return HCNetComponent{};
}

bool World::netCompChangeUniqueId(UniqueId from, UniqueId to) {
  auto it = m_netComps.find(from);
  if (it != m_netComps.end()) {
    auto [_, ok] = m_netComps.insert({to, it->second});
    if (ok) {
      it->second->setUniqueId(to);
      m_netComps.erase(it);
      return true;
    }
  }
  return false;
}

void World::setupMyPlayer() {
  using namespace bs;
  AlfAssert(m_netComps.empty(), "setupmyplayer must be done first");
  HSceneObject player = ObjectBuilder{ObjectType::kEmpty}.build();
  HCharacterController charController =
      player->addComponent<CCharacterController>();
  charController->setHeight(1.0f);
  charController->setRadius(0.4f);
  auto netComp = player->addComponent<CNetComponent>();
  auto camera = createCamera(player);
  m_player = player->addComponent<CMyPlayer>(this);
  auto [it, ok] = m_netComps.insert({UniqueId::invalid(), netComp});
  AlfAssert(ok, "could not create my player");
  m_ui.setup(this, camera, m_width, m_height);
}

void World::applyMoveableState(const MoveableState &moveableState) {
  MICROPROFILE_SCOPEI("world", "appylMoveableState", MP_TURQUOISE);
  auto it = m_netComps.find(moveableState.id);
  if (it != m_netComps.end()) {
    it->second->setState(moveableState);
  } else {
    DLOG_ERROR("failed to find netcomp with id {}", moveableState.id.raw());
  }
}

void World::applyMyMoveableState(const MoveableState &moveableState) {
  MICROPROFILE_SCOPEI("world", "appylMyMoveableState", MP_TURQUOISE1);
  auto it = m_netComps.find(moveableState.id);
  if (it != m_netComps.end()) {
    const auto myPos = it->second->getState().position;
    const auto newPos = moveableState.position;
    constexpr float kDivergeMax = 2.0f;
    if (std::fabs(myPos.x - newPos.x) > kDivergeMax ||
        std::fabs(myPos.y - newPos.y) > kDivergeMax ||
        std::fabs(myPos.z - newPos.z) > kDivergeMax) {
      DLOG_INFO("[client] state diverged, correcting");
      it->second->setPosition(newPos);
    }
  } else {
    DLOG_ERROR("[world:applyMyMoveableState] failed to find netcomp with id {}",
               moveableState.id.raw());
  }
}

void World::onPlayerJoin(const MoveableState &moveableState) {
  DLOG_INFO("player {} joined", moveableState.id.raw());

  if (m_netComps.count(moveableState.id)) {
    DLOG_VERBOSE("[world:onPlayerJoin] duplicate player, not adding, {}",
                 moveableState.id.raw());
    return;
  }

  auto so = ObjectBuilder{ObjectType::kPlayer}
                .withName("playerJoin")
                .withPosition(moveableState.position)
                .withNetComponent(moveableState)
                .build();

  auto netComp = so->getComponent<CNetComponent>();
  auto [it, ok] = getNetComps().insert({moveableState.id, netComp});
  AlfAssert(ok, "failed to create player, was the id unique?");
  so->setParent(m_dynamicScene);
}

void World::onPlayerLeave(UniqueId uid) {
  DLOG_INFO("player {} left", uid.raw());

  {
    auto it = m_netComps.find(uid);
    if (it != m_netComps.end()) {
      it->second->SODestroy();
      m_netComps.erase(it);
    } else {
      DLOG_WARNING("(netComps) player left, but couldn't find them");
    }
  }
}

void World::onPlayerInput(UniqueId uid, PlayerInput input,
                          std::optional<bs::Quaternion> maybeRot) {
  MICROPROFILE_SCOPEI("world", "onPlayerInput", MP_TURQUOISE2);
  auto it = m_netComps.find(uid);
  if (it != m_netComps.end()) {
    auto walker = it->second->SO()->getComponent<FPSWalker>();
    walker->setInput(input);
    if (maybeRot) {
      walker->applyRotation(*maybeRot);
    }
  }
}

void World::onSceneChange(const String &scene) {
  m_staticScene->destroy();
  m_staticScene = Scene::load(scene);
}

void World::onDisconnect() {
  DLOG_VERBOSE("onDisconnect, clearing the world of {} netComps",
               m_netComps.size());

  auto myNetComp = getPlayerNetComp();
  if (myNetComp) {

    const auto myUid = myNetComp->getUniqueId();
    for (auto [uid, netComp] : m_netComps) {
      if (netComp && uid != myUid) {
        netComp->SODestroy();
      }
    }

    m_netComps.clear();
    myNetComp->setUniqueId(UniqueId::invalid());
    m_netComps.insert({UniqueId::invalid(), myNetComp});
  }

  if (m_server.isActive()) {
    m_server.StopServer();
  }

  if (m_staticScene) {
    m_staticScene->destroy();
  }
}

void World::buildObject(const CreateInfo &info) {
  MICROPROFILE_SCOPEI("world", "buildObject", MP_TURQUOISE4);

  if (m_netComps.count(info.state.id)) {
    DLOG_VERBOSE("duplicate object, not building, {}", info.state.id.raw());
    return;
  }

  auto obj = ObjectBuilder(info.type)
                 .withScale(info.scale)
                 .withPosition(info.state.position)
                 .withRotation(info.state.rotation);
  for (const auto &component : info.components) {
    if (component.isType<ComponentData::RigidbodyData>()) {
      obj.withRigidbody();
    } else if (component.isType<ComponentData::WindData>()) {
      DLOG_ERROR("trying to build a windComponet, but not "
                 "supported");
      const auto &wind = component.windSourceData();
      const auto volumeType = WindSystem::u8ToVolumeType(wind.volumeType);
      obj.withWindVolume(volumeType, wind.pos, wind.scale);
      obj.withWindSource(wind.functions, volumeType, wind.pos, wind.scale);
      DLOG_ERROR("withWindSource is currently broke, TODO");
    } else if (component.isType<ComponentData::RenderableData>()) {
      const auto &render = component.renderableData();
      obj.withMaterial(ObjectBuilder::ShaderKind::kStandard,
                       render.pathTexture);
    } else if (component.isType<ComponentData::RotorData>()) {
      const auto &rotor = component.rotorData();
      // TODO how to convert rotor, quat -> vec3
      const Vec3F v{rotor.rot.x, rotor.rot.y, rotor.rot.z};
      obj.withRotor(v);
    } else if (component.isType<ComponentData::ColliderData>()) {
      const auto &collider = component.colliderData();
      obj.withCollider(collider.restitution, collider.mass);
    } else if (component.isType<ComponentData::WindAffectableData>()) {
      obj.withWindAffectable();
    }
  }

  obj.withNetComponent(info.state);

  auto so = obj.build();
  const auto netComp = so->getComponent<CNetComponent>();
  addNetComp(netComp);
  so->setParent(m_dynamicScene);
}

bs::HSceneObject World::createCamera(bs::HSceneObject player) {
  using namespace bs;
  HSceneObject camera = ObjectBuilder{ObjectType::kEmpty}.build();
  camera->setParent(player);
  camera->setPosition(Vector3(0.0f, 1.8f * 0.5f - 0.1f, 0.0f));

  HCamera cameraComp = camera->addComponent<CCamera>();
  SPtr<RenderWindow> primaryWindow = gApplication().getPrimaryWindow();
  cameraComp->getViewport()->setTarget(primaryWindow);
  cameraComp->setMain(true);
  const auto &windowProps = primaryWindow->getProperties();
  float aspectRatio = windowProps.width / (float)windowProps.height;
  cameraComp->setAspectRatio(aspectRatio);
  cameraComp->setProjectionType(PT_PERSPECTIVE);
  cameraComp->setHorzFOV(Degree(90));
  cameraComp->setNearClipDistance(0.005f);
  cameraComp->setFarClipDistance(1000.0f);

  HFPSCamera fpsCameraComp = camera->addComponent<FPSCamera>();
  fpsCameraComp->setCharacter(player);
  m_fpsCamera = fpsCameraComp;

  m_fpsCamera->setEnabled(false);
  Cursor::instance().show();
  Cursor::instance().clipDisable();

  return camera;
}

void World::setupInput() {
  using namespace bs;
  m_scroll = bs::VirtualAxis("Scroll");

  gInput().onButtonUp.connect([this](const ButtonEvent &ev) {
    if (ev.buttonCode == BC_ESCAPE) {
      gApplication().quitRequested();
    }
  });

  gInput().onButtonDown.connect([this](const ButtonEvent &ev) {
    if (ev.buttonCode == BC_MOUSE_RIGHT) {
      if (m_cursorMode) {
        m_cursorMode = !m_cursorMode;
        m_fpsCamera->setEnabled(false);
        Cursor::instance().show();
        Cursor::instance().clipDisable();
      } else {
        m_cursorMode = !m_cursorMode;
        m_fpsCamera->setEnabled(true);
        Cursor::instance().hide();
        SPtr<RenderWindow> primaryWindow = gApplication().getPrimaryWindow();
        Cursor::instance().clipToWindow(*primaryWindow);
      }
    } else if (ev.buttonCode == BC_1) {
      if (m_player->isConnected()) {
        m_player->setWeapon(ObjectType::kInvalid);
      }
    } else if (ev.buttonCode == BC_2) {
      if (m_player->isConnected()) {
        m_player->setWeapon(ObjectType::kBall);
      }
    } else if (ev.buttonCode == BC_3) {
      if (m_player->isConnected()) {
        m_player->setWeapon(ObjectType::kCube);
      }
    } else if (ev.buttonCode == BC_M) {
      if (m_staticScene) {
        Util::dumpScene(m_staticScene);
      }
    } else if (ev.buttonCode == BC_N) {
      if (m_staticScene) {
        DLOG_INFO("{}", Scene::save(m_staticScene));
      }
    } else if (ev.buttonCode == BC_B) {
      if (m_dynamicScene) {
        Util::dumpScene(m_dynamicScene);
      }
    } else if (ev.buttonCode == BC_V) {
      if (m_dynamicScene) {
        DLOG_INFO("{}", Scene::save(m_dynamicScene));
      }
    } else if (ev.buttonCode == BC_C) {
      dumpNetComps();
    }
  });

  auto inputConfig = gVirtualInput().getConfiguration();
  inputConfig->registerButton("Forward", BC_W);
  inputConfig->registerButton("Back", BC_S);
  inputConfig->registerButton("Left", BC_A);
  inputConfig->registerButton("Right", BC_D);
  inputConfig->registerButton("AUp", BC_UP);
  inputConfig->registerButton("ADown", BC_DOWN);
  inputConfig->registerButton("ALeft", BC_LEFT);
  inputConfig->registerButton("ARight", BC_RIGHT);
  inputConfig->registerButton("FastMove", BC_LSHIFT);
  inputConfig->registerButton("Fire1", BC_MOUSE_LEFT);
  inputConfig->registerButton("MRight", BC_MOUSE_RIGHT);
  inputConfig->registerButton("Fire2", BC_MOUSE_MIDDLE);
  inputConfig->registerButton("Space", BC_SPACE);
  inputConfig->registerButton("Gravity", BC_Q);
  inputConfig->registerButton("Fire1", BC_E);
  inputConfig->registerButton("Fire2", BC_R);
  inputConfig->registerAxis("Horizontal",
                            VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
  inputConfig->registerAxis("Vertical",
                            VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));
  inputConfig->registerAxis("Scroll",
                            VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseZ));
}

void World::addNetComp(HCNetComponent netComp) {
  DLOG_VERBOSE("net component with id [{}] added",
               netComp->getUniqueId().raw());
  auto [it, ok] = m_netComps.insert({netComp->getUniqueId(), netComp});
  AlfAssert(ok, "failed to add net comp");
}

void World::scanForNetComps() {
  const auto before = m_netComps.size();

  for (u32 i = 0; i < m_dynamicScene->getNumChildren(); ++i) {
    auto obj = m_dynamicScene->getChild(i);
    auto netComp = obj->getComponent<CNetComponent>();
    addNetComp(netComp);
  }

  const auto after = m_netComps.size();
  DLOG_VERBOSE("added {} netComps", after - before);
}

void World::dumpNetComps() {
  for (auto [uid, netComp] : m_netComps) {
    DLOG_INFO("{}", uid.raw());
  }
}

} // namespace wind
