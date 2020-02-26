#include "world.hpp"
#include "Components/BsCCamera.h"
#include "Components/BsCCharacterController.h"
#include "GUI/BsCGUIWidget.h"
#include "GUI/BsGUIButton.h"
#include "GUI/BsGUIInputBox.h"
#include "GUI/BsGUILayoutX.h"
#include "GUI/BsGUILayoutY.h"
#include "GUI/BsGUIPanel.h"
#include "GUI/BsGUISlider.h"
#include "Importer/BsImporter.h"
#include "Input/BsInput.h"
#include "Platform/BsCursor.h"
#include "Scene/BsSceneManager.h"
#include "shared/asset.hpp"
#include "shared/log.hpp"
#include <Components/BsCSkybox.h>
#include <Image/BsSpriteTexture.h>
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

World::World(const App::Info &info)
    : App(info), m_server(this), m_creator(this) {
  using namespace bs;

  setupMyPlayer();
  setupInput();

  m_creator.floor();
  const HTexture skyboxTex = Asset::loadCubemap("res/skybox/daytime.hdr");
  HSceneObject skybox = SceneObject::create("Skybox");
  HSkybox skyboxComp = skybox->addComponent<CSkybox>();
  skyboxComp->setTexture(skyboxTex);
}

void World::onPreUpdate(f32) {
  MicroProfileFlip(nullptr);
  MICROPROFILE_SCOPEI("world", "onPreUpdate", MP_BLUE2);
  if (m_cursorMode) {
    Util::centerCursor(bs::gApplication().getPrimaryWindow());
  }

  m_server.Poll();
}

void World::onFixedUpdate(f32) {
  using namespace bs;
  MICROPROFILE_SCOPEI("world", "onFixedUpdate", MP_BLUE3);

  m_netDebugInfo.update(m_player->getClient());

  static u32 i = 0;

  if (++i > 2) {
    i = 0;
    m_server.broadcastServerTick(m_netComps);
  }
}

void World::onWindowResize() { updateAimPosition(m_width, m_height); }

void World::setupScene() {
  logVeryVerbose("[world:setupScene] loading scene");
  auto cubeState = MoveableState::generateNew();
  cubeState.setPosition(bs::Vector3(0.0f, 2.0f, -8.0f));
  m_creator.cube(cubeState);
  cubeState = MoveableState::generateNew();
  cubeState.setPosition(bs::Vector3(0.0f, 5.0f, -8.0f));
  cubeState.setRotation(
      bs::Quaternion(bs::Degree(0), bs::Degree(45), bs::Degree(0)));
  m_creator.cube(cubeState);

  auto rotorState = MoveableState::generateNew();
  rotorState.setPosition(bs::Vector3(5.0f, 5.0f, -8.0f));
  m_creator.rotor(rotorState);

  auto windSourceState = MoveableState::generateNew();
  windSourceState.setPosition(bs::Vector3(-5.0f, 2.5f, -8.0f));
  auto r = m_creator.windSource(windSourceState);
  int a = 0;
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
    if (it->second->getState().getType() == ComponentTypes::kPlayer) {
      auto cit = m_walkers.find(from);
      if (cit != m_walkers.end()) {
        auto [_, ok] = m_walkers.insert({to, cit->second});
        if (ok) {
          m_walkers.erase(cit);
        }
      }
    }
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
  HSceneObject player = SceneObject::create("MyPlayer");
  HCharacterController charController =
      player->addComponent<CCharacterController>();
  charController->setHeight(1.0f);
  charController->setRadius(0.4f);
  auto netComp = player->addComponent<CNetComponent>();
  netComp->setType(ComponentTypes::kPlayer);
  auto camera = createCamera(player);
  createGUI(camera);
  m_player = player->addComponent<CMyPlayer>(this);
  auto [it, ok] = m_netComps.insert({UniqueId::invalid(), netComp});
  AlfAssert(ok, "could not create my player");
}

void World::applyMoveableState(const MoveableState &moveableState) {
  MICROPROFILE_SCOPEI("world", "appylMoveableState", MP_TURQUOISE);
  auto it = m_netComps.find(moveableState.getUniqueId());
  if (it != m_netComps.end()) {
    it->second->setState(moveableState);
  } else {
    logVerbose("failed to find netcomp with id {}",
               moveableState.getUniqueId().raw());
    m_player->lookupId(moveableState.getUniqueId());
  }
}

void World::applyMyMoveableState(const MoveableState &moveableState) {
  MICROPROFILE_SCOPEI("world", "appylMyMoveableState", MP_TURQUOISE1);
  auto it = m_netComps.find(moveableState.getUniqueId());
  if (it != m_netComps.end()) {
    const auto myPos = it->second->getState().getPosition();
    const auto newPos = moveableState.getPosition();
    constexpr float kDivergeMax = 2.0f;
    if (std::fabs(myPos.x - newPos.x) > kDivergeMax ||
        std::fabs(myPos.y - newPos.y) > kDivergeMax ||
        std::fabs(myPos.z - newPos.z) > kDivergeMax) {
      logVerbose("[client] state diverged, correcting");
      it->second->setPosition(newPos);
    }
  } else {
    logError("[world:applyMyMoveableState] failed to find netcomp with id {}",
             moveableState.getUniqueId().raw());
  }
}

void World::onPlayerJoin(const MoveableState &moveableState) {
  logInfo("player {} joined", moveableState.getUniqueId().raw());
  m_creator.player(moveableState);
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

  {
    auto it = m_walkers.find(uid);
    if (it != m_walkers.end()) {
      m_walkers.erase(it);
    } else {
      logWarning("(characters) player left, but couldn't find them");
    }
  }
}

void World::onPlayerInput(UniqueId uid, PlayerInput input,
                          std::optional<bs::Quaternion> maybeRot) {
  MICROPROFILE_SCOPEI("world", "onPlayerInput", MP_TURQUOISE2);
  auto it = m_walkers.find(uid);
  if (it != m_walkers.end()) {
    it->second->setInput(input);
    if (maybeRot) {
      it->second->applyRotation(*maybeRot);
    }
    // TODO
    // it->second->resetNewInputFlag();
    // m_netComps[uid]->resetChanged();
  }
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
    m_walkers.clear();
    myNetComp->setUniqueId(UniqueId::invalid());
    m_netComps.insert({UniqueId::invalid(), myNetComp});
    m_walkers.insert({UniqueId::invalid(), m_player->getWalker()});
  }
}

bs::HSceneObject World::createCamera(bs::HSceneObject player) {
  using namespace bs;
  HSceneObject camera = SceneObject::create("Camera");
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
        m_player->setWeapon(ComponentTypes::kInvalid);
      }
    } else if (ev.buttonCode == BC_2) {
      if (m_player->isConnected()) {
        m_player->setWeapon(ComponentTypes::kBall);
      }
    } else if (ev.buttonCode == BC_3) {
      if (m_player->isConnected()) {
        m_player->setWeapon(ComponentTypes::kCube);
      }
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
      if (m_server.getConnectionState() == ConnectionState::kDisconnected) {
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
        rotor->setRotation(percent * percent * 1000);
      }
    });
  }

  { // shootForce slider
    auto l = layout->addNewElement<GUILayoutX>();
    auto text = l->addNewElement<GUILabel>(HString{u8"shootForce"});
    text->setWidth(70);
    auto slider = l->addNewElement<GUISliderHorz>();
    slider->setPercent(0.3f);
    slider->onChanged.connect(
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

} // namespace wind
