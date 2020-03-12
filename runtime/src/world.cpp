#include "world.hpp"
#include "shared/asset.hpp"
#include "shared/log.hpp"
#include "shared/math/math.hpp"
#include "shared/scene/builder.hpp"
#include "shared/scene/scene.hpp"
#include "shared/scene/wind_src.hpp"
#include "shared/utility/util.hpp"

#include <Components/BsCCamera.h>
#include <Components/BsCCharacterController.h>
#include <GUI/BsCGUIWidget.h>
#include <GUI/BsGUIButton.h>
#include <GUI/BsGUIInputBox.h>
#include <GUI/BsGUILayoutX.h>
#include <GUI/BsGUILayoutY.h>
#include <GUI/BsGUIPanel.h>
#include <GUI/BsGUISlider.h>
#include <Image/BsSpriteTexture.h>
#include <Importer/BsImporter.h>
#include <Input/BsInput.h>
#include <Math/BsQuaternion.h>
#include <Platform/BsCursor.h>
#include <Scene/BsSceneManager.h>
#include <microprofile/microprofile.h>
#include <regex>

namespace wind {

void NetDebugInfo::setup(bs::GUILayoutY *layout) {
  hCQL = layout->addNewElement<bs::GUILabel>(bs::HString(CQL));
  hCQR = layout->addNewElement<bs::GUILabel>(bs::HString(CQR));
  hBSCE = layout->addNewElement<bs::GUILabel>(bs::HString(BSCE));
  hping = layout->addNewElement<bs::GUILabel>(bs::HString(ping));
  houtBytes = layout->addNewElement<bs::GUILabel>(bs::HString(outBytes));
  houtPackets = layout->addNewElement<bs::GUILabel>(bs::HString(outPackets));
  hinBytes = layout->addNewElement<bs::GUILabel>(bs::HString(inBytes));
  hinPackets = layout->addNewElement<bs::GUILabel>(bs::HString(inPackets));
  hqueueTime = layout->addNewElement<bs::GUILabel>(bs::HString(queueTime));
}

void NetDebugInfo::update(const Client &client) {
  MICROPROFILE_SCOPEI("netdebuginfo", "update", MP_GREEN3);
  auto status = client.GetConnectionStatus();
  if (!status) {
    return;
  }

  CQL = fmt::format("CQL {:.2f}", status->m_flConnectionQualityLocal);
  CQR = fmt::format("CQR {:.2f}", status->m_flConnectionQualityRemote);
  BSCE =
      fmt::format("b/s cap estimation {}", status->m_nSendRateBytesPerSecond);
  ping = fmt::format("ping {}", status->m_nPing);
  outBytes = fmt::format("out b/s {:.2f}", status->m_flOutBytesPerSec);
  outPackets = fmt::format("out p/s {:.2f}", status->m_flOutPacketsPerSec);
  inBytes = fmt::format("in  b/s {}", status->m_flInBytesPerSec);
  inPackets = fmt::format("in  p/s {}", status->m_flInPacketsPerSec);
  queueTime = fmt::format("q time {}us", status->m_usecQueueTime);

  hCQL->setContent(bs::HString(CQL));
  hCQR->setContent(bs::HString(CQR));
  hBSCE->setContent(bs::HString(BSCE));
  hping->setContent(bs::HString(ping));
  houtBytes->setContent(bs::HString(outBytes));
  houtPackets->setContent(bs::HString(outPackets));
  hinBytes->setContent(bs::HString(inBytes));
  hinPackets->setContent(bs::HString(inPackets));
  hqueueTime->setContent(bs::HString(queueTime));
}

// ============================================================ //

constexpr u32 kAimDiameter = 8;

// ============================================================ //

World::World(const App::Info &info) : App(info), m_server(this) {
  using namespace bs;

  setupMyPlayer();
  setupInput();

  std::filesystem::file_time_type time =
      std::filesystem::last_write_time(m_scenePath);
  m_sceneEditTime = time;
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
    const f32 new_percent = m_shootForce->getPercent() + v;
    m_shootForce->setPercent(new_percent);
    m_player->setShootForce(100.0f * new_percent);
  }
}

void World::onFixedUpdate(f32) {
  using namespace bs;
  MICROPROFILE_SCOPEI("world", "onFixedUpdate", MP_BLUE3);

  if (m_player) {
    m_netDebugInfo.update(m_player->getClient());
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

void World::onWindowResize() { updateAimPosition(m_width, m_height); }

void World::setupScene() {
  logVeryVerbose("[world:setupScene] loading scene");
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

    logInfo(
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
  } else {
    logWarning("[world:reloadStaticScene] cannot reload scene because we have "
               "no active server");
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
  createGUI(camera);
  m_player = player->addComponent<CMyPlayer>(this);
  auto [it, ok] = m_netComps.insert({UniqueId::invalid(), netComp});
  AlfAssert(ok, "could not create my player");
}

void World::applyMoveableState(const MoveableState &moveableState) {
  MICROPROFILE_SCOPEI("world", "appylMoveableState", MP_TURQUOISE);
  auto it = m_netComps.find(moveableState.id);
  if (it != m_netComps.end()) {
    it->second->setState(moveableState);
  } else {
    logError("failed to find netcomp with id {}", moveableState.id.raw());
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
      logVerbose("[client] state diverged, correcting");
      it->second->setPosition(newPos);
    }
  } else {
    logError("[world:applyMyMoveableState] failed to find netcomp with id {}",
             moveableState.id.raw());
  }
}

void World::onPlayerJoin(const MoveableState &moveableState) {
  logInfo("player {} joined", moveableState.id.raw());

  if (m_netComps.count(moveableState.id)) {
    logVeryVerbose("[world:onPlayerJoin] duplicate player, not adding, {}",
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
  logInfo("player {} left", uid.raw());

  {
    auto it = m_netComps.find(uid);
    if (it != m_netComps.end()) {
      it->second->SODestroy();
      m_netComps.erase(it);
    } else {
      logWarning("(netComps) player left, but couldn't find them");
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
  logVerbose("[world] onDisconnect, clearing the world of {} netComps",
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
    logVeryVerbose("[world:buildObject] duplicate object, not building, {}",
                   info.state.id.raw());
    return;
  }

  auto obj = ObjectBuilder(info.type)
                 .withScale(info.scale)
                 .withPosition(info.state.position)
                 .withRotation(info.state.rotation);
  for (const auto &component : info.components) {
    if (component.isType<ComponentData::RigidbodyData>()) {
      MICROPROFILE_SCOPEI("World", "RigidbodyData", MP_TURQUOISE4);
      obj.withRigidbody();
    } else if (component.isType<ComponentData::WindSourceData>()) {
      MICROPROFILE_SCOPEI("World", "WindSourceData", MP_TURQUOISE4);
      // const auto &wind = component.windSourceData();
      // obj.withWindSource(wind.functions);
      logError("[world:buildObject] withWindSource is currently broke, TODO");
    } else if (component.isType<ComponentData::RenderableData>()) {
      MICROPROFILE_SCOPEI("World", "RenderableData", MP_TURQUOISE4);
      const auto &render = component.renderableData();
      obj.withMaterial(ObjectBuilder::ShaderKind::kStandard,
                       render.pathTexture);
    } else if (component.isType<ComponentData::RotorData>()) {
      MICROPROFILE_SCOPEI("World", "RotorData", MP_TURQUOISE4);
      const auto &rotor = component.rotorData();
      // TODO how to convert rotor, quat -> vec3
      Vec3F v{rotor.rot.x, rotor.rot.y, rotor.rot.z};
      obj.withRotor(v);
    } else if (component.isType<ComponentData::ColliderData>()) {
      MICROPROFILE_SCOPEI("World", "ColliderData", MP_TURQUOISE4);
      const auto &collider = component.colliderData();
      obj.withCollider(collider.restitution, collider.mass);
    }
  }

  obj.withNetComponent(info.state);

  auto so = obj.build();
  auto netComp = so->getComponent<CNetComponent>();
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
      Util::dumpScene(m_staticScene);
    } else if (ev.buttonCode == BC_N) {
      logInfo("{}", Scene::save(m_staticScene));
    } else if (ev.buttonCode == BC_B) {
      Util::dumpScene(m_dynamicScene);
    } else if (ev.buttonCode == BC_V) {
      logInfo("{}", Scene::save(m_dynamicScene));
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

bs::HSceneObject World::createGUI(bs::HSceneObject camera) {
  using namespace bs;

  auto cameraComp = camera->getComponent<CCamera>();

  auto gui = SceneObject::create("GUI");
  HGUIWidget guiComp = gui->addComponent<CGUIWidget>(cameraComp);
  GUIPanel *mainPanel = guiComp->getPanel();
  GUILayoutY *layout = mainPanel->addNewElement<GUILayoutY>();
  layout->setSize(300, 700);
  layout->addNewElement<GUILabel>(HString{u8"Press the Escape key to quit"});

  { // Start Server
    GUILayoutX *l = layout->addNewElement<GUILayoutX>();
    l->addNewElement<GUILabel>(HString{u8"port"});
    GUIInputBox *input = l->addNewElement<GUIInputBox>();
    input->setText("4040");
    input->setFilter([](const String &str) {
      return std::regex_match(str, std::regex("-?(\\d+)?"));
    });

    GUIButton *startServerBtn =
        l->addNewElement<GUIButton>(GUIContent{HString{"start server"}});
    startServerBtn->onClick.connect([input, this] {
      if (!m_server.isActive()) {
        logVerbose("start server on {}", input->getText().c_str());
        m_server.StartServer(std::atoi(input->getText().c_str()));
      }
    });
  }

  { // Client network
    GUILayoutX *l = layout->addNewElement<GUILayoutX>();
    l->addNewElement<GUILabel>(HString{u8"ip:port"});
    GUIInputBox *input = l->addNewElement<GUIInputBox>();
    input->setText("127.0.0.1:4040");

    GUIButton *btn =
        l->addNewElement<GUIButton>(GUIContent{HString{"connect"}});
    btn->onClick.connect([input, this] {
      if (!m_player->isConnected()) {
        logVerbose("connect to {}", input->getText().c_str());
        auto &t = input->getText();
        if (t.find(":") != std::string::npos && t.size() > 8) {
          m_player->connect(input->getText().c_str());
          // setupScene();
        }
      }
    });
    GUIButton *btn2 = l->addNewElement<GUIButton>(GUIContent{HString{"d/c"}});
    btn2->onClick.connect([input, this] {
      if (m_player->isConnected()) {
        m_player->disconnect();
      }
    });
  }

  { // network info
    m_netDebugInfo.setup(layout);
  }

  { // rotor slider
    auto l = layout->addNewElement<GUILayoutX>();
    auto text = l->addNewElement<GUILabel>(HString{u8"rotorSpeed"});
    text->setWidth(70);
    auto slider = l->addNewElement<GUISliderHorz>();
    slider->onChanged.connect([](f32 percent) {
      auto rotors = gSceneManager().findComponents<CRotor>(true);
      for (auto &rotor : rotors) {
        rotor->setRotation(bs::Vector3{0.0f, percent * percent * 1000, 0.0f});
      }
    });
  }

  { // shootForce slider
    auto l = layout->addNewElement<GUILayoutX>();
    auto text = l->addNewElement<GUILabel>(HString{u8"shootForce"});
    text->setWidth(70);
    m_shootForce = l->addNewElement<GUISliderHorz>();
    m_shootForce->setPercent(0.3f);
    m_shootForce->onChanged.connect(
        [this](f32 percent) { m_player->setShootForce(100.0f * percent); });
  }

  { // aim dot
    HTexture dotTex = gImporter().import<Texture>("res/textures/dot.png");
    HSpriteTexture dotSprite = SpriteTexture::create(dotTex);
    m_aim = GUITexture::create(dotSprite);
    m_aim->setSize(kAimDiameter, kAimDiameter);
    updateAimPosition(m_width, m_height);
    mainPanel->addElement(m_aim);
  }

  return gui;
}

void World::updateAimPosition(u32 width, u32 height) {
  m_aim->setPosition(width / 2 - kAimDiameter / 2,
                     height / 2 - kAimDiameter / 2);
}

void World::addNetComp(HCNetComponent netComp) {
  logVerbose("net component with id [{}] added", netComp->getUniqueId().raw());
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
  logVerbose("[world:scanForNetComps] added {} netComps", after - before);
}

void World::dumpNetComps() {
  for (auto [uid, netComp] : m_netComps) {
    logInfo("{}", uid.raw());
  }
}

} // namespace wind
