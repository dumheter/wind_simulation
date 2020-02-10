#include "world.hpp"

#include "BsFPSCamera.h"
#include "Components/BsCBoxCollider.h"
#include "Components/BsCCamera.h"
#include "Components/BsCCharacterController.h"
#include "Components/BsCPlaneCollider.h"
#include "Components/BsCRenderable.h"
#include "Components/BsCRigidbody.h"
#include "GUI/BsCGUIWidget.h"
#include "GUI/BsGUILabel.h"
#include "GUI/BsGUILayout.h"
#include "GUI/BsGUILayoutY.h"
#include "GUI/BsGUIPanel.h"
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
#include <Components/BsCSkybox.h>
#include <alflib/core/assert.hpp>

namespace wind {

World::World(const App::Info &info) : App(info) {
  setupInput();
  setupScene();
}

void World::setupScene() {
  using namespace bs;
  auto floorMaterial = createMaterial("res/textures/grid.png");
  auto cubeMaterial = createMaterial("res/textures/grid_2.png");
  auto stdPhysicsMaterial = createPhysicsMaterial();
  auto cube = createCube(cubeMaterial, stdPhysicsMaterial);
  cube->setPosition(Vector3(0.0f, 2.0f, -8.0f));
  auto cube2 = createCube(cubeMaterial, stdPhysicsMaterial);
  cube2->setPosition(Vector3(0.0f, 4.0f, -8.0f));
  cube2->setRotation(Quaternion(Degree(0), Degree(45), Degree(0)));
  auto floor = createFloor(floorMaterial, stdPhysicsMaterial);
  const HTexture skyboxTex = Asset::loadCubemap("res/skybox/daytime.hdr");
  HSceneObject skybox = SceneObject::create("Skybox");
  HSkybox skyboxComp = skybox->addComponent<CSkybox>();
  skyboxComp->setTexture(skyboxTex);
}

bs::HSceneObject World::setupMyPlayer() {
  using namespace bs;
  AlfAssert(m_netComps.empty(),
            "m_netComps must be empty before setting up myplayer");

  HSceneObject player = SceneObject::create("Player");
  HCharacterController charController =
      player->addComponent<CCharacterController>();
  charController->setHeight(1.0f);
  charController->setRadius(0.4f);
  player->addComponent<FPSWalker>();
  auto camera = createCamera(player);
  auto gui = createGUI(camera);
  auto cmyplayer = player->addComponent<CMyPlayer>(this);
  // m_players.push_back(player);
  return player;
}

bs::HSceneObject World::setupPlayer() {
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
  HShader shader =
      gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
  HMaterial material = Material::create(shader);
  HTexture texture = Asset::loadTexture("res/grid_2.png");
  material->setTexture("gAlbedoTex", texture);
  renderable->setMesh(mesh);
  renderable->setMaterial(material);
  HCNetComponent netComp = player->addComponent<CNetComponent>();
  m_netComps.push_back(netComp);
  return player;
}

void World::reset() {
  for (auto netComp : m_netComps) {
    netComp->destroy();
  }
  m_netComps.clear();
}

void World::onPlayerJoin(UniqueId playerUid) { logInfo("player joined TODO"); }

void World::onPlayerLeave(UniqueId uid) {}

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

  Cursor::instance().hide();
  Cursor::instance().clipToWindow(*primaryWindow);

  return camera;
}

void World::setupInput() {
  using namespace bs;
  gInput().onButtonUp.connect([this](const ButtonEvent &ev) {
    if (ev.buttonCode == BC_ESCAPE) {
      gApplication().quitRequested();
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

bs::HSceneObject World::createCube(bs::HMaterial material,
                                   bs::HPhysicsMaterial physicsMaterial) {
  using namespace bs;
  HSceneObject cube = SceneObject::create("Cube");
  HRenderable renderable = cube->addComponent<CRenderable>();
  HMesh mesh = gBuiltinResources().getMesh(BuiltinMesh::Box);
  renderable->setMesh(mesh);
  renderable->setMaterial(material);
  HBoxCollider boxCollider = cube->addComponent<CBoxCollider>();
  boxCollider->setMaterial(physicsMaterial);
  boxCollider->setMass(25.0f);
  HRigidbody boxRigidbody = cube->addComponent<CRigidbody>();
  return cube;
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
  auto gui = SceneObject::create("GUI");
  auto cameraComp = camera->getComponent<CCamera>();
  HGUIWidget guiComp = gui->addComponent<CGUIWidget>(cameraComp);
  GUIPanel *mainPanel = guiComp->getPanel();
  GUILayoutY *vertLayout = GUILayoutY::create();

  HString quitString(u8"Press the Escape key to quit");

  vertLayout->addNewElement<GUILabel>(quitString);
  mainPanel->addElement(vertLayout);
  return gui;
}

void World::addNetComp(HCNetComponent netComp) {
  logVerbose("net component with id [{}] added", netComp->getUniqueId());
  m_netComps.push_back(netComp);
}

} // namespace wind
