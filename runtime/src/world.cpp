#include "world.hpp"
#include "BsApplication.h"
#include "BsFPSCamera.h"
#include "Components/BsCBoxCollider.h"
#include "Components/BsCCamera.h"
#include "Components/BsCCharacterController.h"
#include "Components/BsCPlaneCollider.h"
#include "Components/BsCRenderable.h"
#include "Components/BsCRigidbody.h"
#include "Components/BsCSphereCollider.h"
#include "GUI/BsCGUIWidget.h"
#include "GUI/BsGUIButton.h"
#include "GUI/BsGUIContent.h"
#include "GUI/BsGUIInputBox.h"
#include "GUI/BsGUILabel.h"
#include "GUI/BsGUILayout.h"
#include "GUI/BsGUILayoutX.h"
#include "GUI/BsGUILayoutY.h"
#include "GUI/BsGUIPanel.h"
#include "GUI/BsGUIViewport.h"
#include "Importer/BsImporter.h"
#include "Input/BsInput.h"
#include "Material/BsMaterial.h"
#include "Physics/BsBoxCollider.h"
#include "Physics/BsPhysicsMaterial.h"
#include "Physics/BsPlaneCollider.h"
#include "Physics/BsRigidbody.h"
#include "Platform/BsCursor.h"
#include "RenderAPI/BsRenderTarget.h"
#include "RenderAPI/BsRenderWindow.h"
#include "Resources/BsBuiltinResources.h"
#include "Scene/BsSceneObject.h"
#include "asset.hpp"
#include "bsFPSWalker.h"
#include "cmyplayer.hpp"
#include "log.hpp"
#include "util.hpp"
#include <Components/BsCSkybox.h>
#include <alflib/core/assert.hpp>
#include <chrono>
#include <cstdlib>
#include <regex>

namespace wind {

World::World(const App::Info &info) : App(info), m_server(this) {
  using namespace bs;
  m_matGrid = createMaterial("res/textures/grid.png");
  m_matGrid2 = createMaterial("res/textures/grid_2.png");
  m_physicsMatStd = createPhysicsMaterial();
  m_meshBall = gBuiltinResources().getMesh(BuiltinMesh::Sphere);

  setupMyPlayer();
  setupInput();

  auto floor = createFloor(m_matGrid, m_physicsMatStd);
  const HTexture skyboxTex = Asset::loadCubemap("res/skybox/daytime.hdr");
  HSceneObject skybox = SceneObject::create("Skybox");
  HSkybox skyboxComp = skybox->addComponent<CSkybox>();
  skyboxComp->setTexture(skyboxTex);
}

void World::onPreUpdate() {
  if (m_cursorMode) {
    Util::CenterCursor(bs::gApplication().getPrimaryWindow());
  }

  m_server.Poll();
}

void World::onFixedUpdate() {
  using namespace std::chrono;

  m_server.broadcastServerTick(m_netComps);
}

void World::setupScene() {
  using namespace bs;

  auto cubeState = MoveableState::generateNew();
  cubeState.setPosition(Vector3(0.0f, 2.0f, -8.0f));
  addCube(cubeState);
  // cubeState = MoveableState::generateNew();
  // cubeState.setPosition(Vector3(0.0f, 4.0f, -8.0f));
  // cubeState.setRotation(Quaternion(Degree(0), Degree(45), Degree(0)));
  // addCube(cubeState);
}

HCNetComponent World::getPlayerNetComp() {
  auto uid = m_player->getUniqueId();
  return m_netComps[uid];
}

bool World::netCompChangeUniqueId(UniqueId from, UniqueId to) {
  auto it = m_netComps.find(from);
  if (it != m_netComps.end()) {
    auto [_, ok] = m_netComps.insert({to, it->second});
    if (ok) {
      it->second->getState().setUniqueId(to);
      m_netComps.erase(it);
      return true;
    }
  }
  return false;
}

void World::setupMyPlayer() {
  using namespace bs;
  AlfAssert(m_netComps.empty(), "must be done first");
  HSceneObject player = SceneObject::create("MyPlayer");
  HCharacterController charController =
      player->addComponent<CCharacterController>();
  charController->setHeight(1.0f);
  charController->setRadius(0.4f);
  auto netComp = player->addComponent<CNetComponent>();
  player->addComponent<FPSWalker>();
  auto camera = createCamera(player);
  auto gui = createGUI(camera);
  m_player = player->addComponent<CMyPlayer>(this);
  auto [it, ok] = m_netComps.insert({UniqueId::kInvalid, netComp});
  AlfAssert(ok, "could not create my player");
}

void World::addPlayer(const MoveableState &moveableState) {
  using namespace bs;
  AlfAssert(!m_netComps.empty(),
            "m_netComps must not be empty before setting up a player");

  HSceneObject player = SceneObject::create("Player");
  HCharacterController charController =
      player->addComponent<CCharacterController>();
  charController->setHeight(1.0f);
  charController->setRadius(0.4f);
  HRenderable renderable = player->addComponent<CRenderable>();
  HMesh mesh = gBuiltinResources().getMesh(BuiltinMesh::Cylinder);
  // HShader shader =
  //     gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
  renderable->setMesh(mesh);
  renderable->setMaterial(m_matGrid2);
  HCNetComponent netComp = player->addComponent<CNetComponent>(moveableState);
  auto [it, ok] = m_netComps.insert({netComp->getUniqueId(), netComp});
  AlfAssert(ok, "failed to add player");
}

void World::addCube(const MoveableState &moveableState) {
  using namespace bs;
  HSceneObject cube = SceneObject::create("Cube");

  HRenderable renderable = cube->addComponent<CRenderable>();
  HMesh mesh = gBuiltinResources().getMesh(BuiltinMesh::Box);
  renderable->setMesh(mesh);
  renderable->setMaterial(m_matGrid2);
  HBoxCollider boxCollider = cube->addComponent<CBoxCollider>();
  boxCollider->setMaterial(m_physicsMatStd);
  boxCollider->setMass(25.0f);
  HRigidbody boxRigidbody = cube->addComponent<CRigidbody>();
  auto netComp = cube->addComponent<CNetComponent>(moveableState);
  auto [it, ok] = m_netComps.insert({moveableState.getUniqueId(), netComp});
  //cube->setPosition(moveableState.getPosition());
  //cube->setRotation(moveableState.getRotation());
  //cube->setScale(moveableState.getScale());
  AlfAssert(ok, "failed to create cube, was the id unique?");
}

void World::addBall(const MoveableState &moveableState) {
  using namespace bs;
  HSceneObject sphere = SceneObject::create("Sphere");
  HRenderable renderable = sphere->addComponent<CRenderable>();
  renderable->setMesh(m_meshBall);
  renderable->setMaterial(m_matGrid2);
  HSphereCollider collider = sphere->addComponent<CSphereCollider>();
  collider->setMaterial(m_physicsMatStd);
  collider->setMass(8.0f);
  HRigidbody rigid = sphere->addComponent<CRigidbody>();
  sphere->addComponent<CNetComponent>(moveableState);
  // rigid->addForce(forward * 40.0f, ForceMode::Velocity);
}

void World::applyMoveableState(const MoveableState &moveableState) {
  auto it = m_netComps.find(moveableState.getUniqueId());
  if (it != m_netComps.end()) {
    it->second->getState().from(moveableState);
  } else {
    logError("failed to find netcomp with id {}",
             moveableState.getUniqueId().raw());
  }
}

void World::reset() {
  // TODO
  // m_netComps.clear();
}

void World::onPlayerJoin(const MoveableState &moveableState) {
  addPlayer(moveableState);
}

void World::onPlayerLeave(UniqueId uid) { logInfo("player leave TODO"); }

bs::HSceneObject World::createCamera(bs::HSceneObject player) {
  using namespace bs;
  HSceneObject camera = SceneObject::create("Camera");
  camera->setParent(player);
  camera->setPosition(Vector3(0.0f, 1.8f * 0.5f - 0.1f, 0.0f));

  HCamera cameraComp = camera->addComponent<CCamera>();
  SPtr<RenderWindow> primaryWindow = gApplication().getPrimaryWindow();
  cameraComp->getViewport()->setTarget(primaryWindow);
  cameraComp->setMain(true);
  auto &windowProps = primaryWindow->getProperties();
  float aspectRatio = windowProps.width / (float)windowProps.height;
  cameraComp->setAspectRatio(aspectRatio);
  cameraComp->setProjectionType(PT_PERSPECTIVE);
  cameraComp->setHorzFOV(Degree(90));
  cameraComp->setNearClipDistance(0.005f);
  cameraComp->setFarClipDistance(1000.0f);

  HFPSCamera fpsCameraComp = camera->addComponent<FPSCamera>();
  fpsCameraComp->setCharacter(player);
  m_fpsCamera = fpsCameraComp;

  Cursor::instance().hide();
  Cursor::instance().clipToWindow(*primaryWindow);

  return camera;
}

void World::setupInput() {
  using namespace bs;
  gInput().onButtonUp.connect([](const ButtonEvent &ev) {
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
    }
    if (ev.buttonCode == BC_MOUSE_LEFT) {
      if (m_player->isConnected() && m_cursorMode) {
        // auto spawnPos = getPlayerNetComp()->getState().getPosition();
        // const auto &forward =
        //     m_fpsCamera->getCamera()->getTransform().getForward();
        // spawnPos += forward * 0.7f;
        // spawnPos += Vector3(0.0f, 1.0f, 0.0f);
        // sphere->setPosition(spawnPos);
        // sphere->setScale(Vector3(0.3f, 0.3f, 0.3f));
        // addBall();
      }
    }
  });

  auto inputConfig = gVirtualInput().getConfiguration();
  inputConfig->registerButton("Forward", BC_W);
  inputConfig->registerButton("Back", BC_S);
  inputConfig->registerButton("Left", BC_A);
  inputConfig->registerButton("Right", BC_D);
  inputConfig->registerButton("Forward", BC_UP);
  inputConfig->registerButton("Back", BC_DOWN);
  inputConfig->registerButton("Left", BC_LEFT);
  inputConfig->registerButton("Right", BC_RIGHT);
  inputConfig->registerButton("FastMove", BC_LSHIFT);
  inputConfig->registerButton("RotateObj", BC_MOUSE_LEFT);
  inputConfig->registerButton("RotateCam", BC_MOUSE_RIGHT);
  inputConfig->registerButton("Space", BC_SPACE);
  inputConfig->registerButton("Gravity", BC_Q);
  inputConfig->registerAxis("Horizontal",
                            VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
  inputConfig->registerAxis("Vertical",
                            VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));
}

bs::HSceneObject World::createFloor(bs::HMaterial material,
                                    bs::HPhysicsMaterial physicsMaterial) {
  using namespace bs;
  HSceneObject floor = SceneObject::create("Floor");
  HRenderable floorRenderable = floor->addComponent<CRenderable>();
  HMesh planeMesh = gBuiltinResources().getMesh(BuiltinMesh::Quad);
  floorRenderable->setMaterial(material);
  floorRenderable->setMesh(planeMesh);
  HPlaneCollider planeCollider = floor->addComponent<CPlaneCollider>();
  planeCollider->setMaterial(physicsMaterial);
  constexpr float kSize = 50.0f;
  floor->setScale(Vector3(kSize, 1.0f, kSize));
  material->setVec2("gUVTile", Vector2::ONE - (Vector2::ONE * -kSize));
  return floor;
}

bs::HPhysicsMaterial World::createPhysicsMaterial() {
  using namespace bs;
  HPhysicsMaterial physicsMaterial = PhysicsMaterial::create(1.0f, 1.0f, 0.5f);
  return physicsMaterial;
}

bs::HMaterial World::createMaterial(const bs::String &path) {
  using namespace bs;
  HShader shader =
      gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
  HMaterial material = Material::create(shader);
  HTexture texture = Asset::loadTexture(path);
  material->setTexture("gAlbedoTex", texture);
  return material;
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
        logVerbose("diconnected");
        m_player->disconnect();
      }
    });
  }

  return gui;
}

void World::addNetComp(HCNetComponent netComp) {
  logVerbose("net component with id [{}] added", netComp->getUniqueId());
  auto [it, ok] = m_netComps.insert({netComp->getUniqueId(), netComp});
  AlfAssert(ok, "failed to add net comp");
}

} // namespace wind
