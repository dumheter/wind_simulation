// MIT License
//
// Copyright (c) 2020 Filip Björklund, Christoffer Gustafsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "editor.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "asset.hpp"
#include "math/obstruction_field.hpp"
#include "math/vector_field.hpp"

#include <BsCameraFlyer.h>
#include <Components/BsCBoxCollider.h>
#include <Components/BsCCamera.h>
#include <Components/BsCMeshCollider.h>
#include <Components/BsCPlaneCollider.h>
#include <Components/BsCRenderable.h>
#include <Components/BsCRigidbody.h>
#include <Components/BsCSkybox.h>
#include <Debug/BsDebugDraw.h>
#include <GUI/BsCGUIWidget.h>
#include <GUI/BsGUIButton.h>
#include <GUI/BsGUIInputBox.h>
#include <GUI/BsGUILabel.h>
#include <GUI/BsGUILayout.h>
#include <GUI/BsGUILayoutY.h>
#include <GUI/BsGUIPanel.h>
#include <GUI/BsGUIProgressBar.h>
#include <GUI/BsGUIToggle.h>
#include <Importer/BsImporter.h>
#include <Importer/BsMeshImportOptions.h>
#include <Importer/BsTextureImportOptions.h>
#include <Input/BsInput.h>
#include <Material/BsMaterial.h>
#include <Mesh/BsMesh.h>
#include <Physics/BsPhysics.h>
#include <Physics/BsPhysicsMaterial.h>
#include <Resources/BsBuiltinResources.h>
#include <Resources/BsResourceManifest.h>
#include <Resources/BsResources.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// Editor Implementation
// ========================================================================== //

namespace wind {

Editor::Editor() : App(Info{"Editor", WINDOW_WIDTH, WINDOW_HEIGHT}) {
  using namespace bs;

  registerControls();
  setupCamera();
  setupScene();
  setupGUI();

  DebugDraw::instance().setColor(Color::Red);
  m_obstrField = ObstructionField::buildForScene(
      SceneManager::instance().getMainScene(),
      Vector3(GROUND_PLANE_SCALE * 2, 5, GROUND_PLANE_SCALE * 2), Vector3(),
      0.5f);
  m_obstrField->debugDraw(Vector3(), false);
}

// -------------------------------------------------------------------------- //

void Editor::OnPreUpdate() {
  using namespace bs;

  if (gInput().isButtonDown(ButtonCode::BC_ESCAPE)) {
    gApplication().quitRequested();
  }
}

// -------------------------------------------------------------------------- //

void Editor::setupCamera() {
  using namespace bs;

  const SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
  const RenderWindowProperties &windowProp = window->getProperties();

  // Skybox
  const HTexture skyboxTex = Asset::loadCubemap("res/skybox/daytime.hdr");
  HSceneObject skybox = SceneObject::create("Skybox");
  HSkybox skyboxComp = skybox->addComponent<CSkybox>();
  skyboxComp->setTexture(skyboxTex);

  // Camera
  m_camera = SceneObject::create("Camera");
  m_camera->setPosition(
      Vector3(GROUND_PLANE_SCALE, 2.5f, GROUND_PLANE_SCALE - 4.0f) * 0.65f);
  m_camera->lookAt(Vector3(.0f, 1.5f, .0f));
  HCamera cameraComp = m_camera->addComponent<CCamera>();
  cameraComp->getViewport()->setTarget(window);
  cameraComp->setNearClipDistance(0.005f);
  cameraComp->setFarClipDistance(1000);
  cameraComp->setAspectRatio(windowProp.width / f32(windowProp.height));
  const SPtr<RenderSettings> renderSettings = cameraComp->getRenderSettings();
  renderSettings->enableIndirectLighting = true;
  cameraComp->setRenderSettings(renderSettings);

  // Flying camera
  const HCameraFlyer cameraFlyerComp = m_camera->addComponent<CameraFlyer>();
}

// -------------------------------------------------------------------------- //

void Editor::setupScene() {
  using namespace bs;

  // Load textures
  const HTexture texGrid = Asset::loadTexture("res/textures/grid.png");
  const HTexture texGrid2 = Asset::loadTexture("res/textures/grid_2.png");

  const HShader shader =
      gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);

  // Setup plane
  HMaterial planeMat = Material::create(shader);
  planeMat->setTexture("gAlbedoTex", texGrid);
  planeMat->setVec2("gUVTile", Vector2::ONE * GROUND_PLANE_SCALE * 4.0f);
  const HMesh planeMesh = gBuiltinResources().getMesh(BuiltinMesh::Quad);

  HSceneObject plane = SceneObject::create("Plane");
  plane->setScale(Vector3(GROUND_PLANE_SCALE, 1.0f, GROUND_PLANE_SCALE));
  plane->move(Vector3(GROUND_PLANE_SCALE, .4f, GROUND_PLANE_SCALE));
  HRenderable planeRenderable = plane->addComponent<CRenderable>();
  planeRenderable->setMesh(planeMesh);
  planeRenderable->setMaterial(planeMat);
  HPhysicsMaterial planePhysMat = PhysicsMaterial::create(1.0f, 1.0f, 0.5f);
  HPlaneCollider planeCollider = plane->addComponent<CPlaneCollider>();
  planeCollider->setMaterial(planePhysMat);
  planeCollider->setMass(0.0f);

  // Setup box
  HMaterial boxMat = Material::create(shader);
  boxMat->setTexture("gAlbedoTex", texGrid2);
  const HMesh boxMesh = gBuiltinResources().getMesh(BuiltinMesh::Box);
  HSceneObject box = SceneObject::create("Box");
  box->setPosition(Vector3(2.0f, 3.0f, 2.0f));
  HRenderable boxRenderable = box->addComponent<CRenderable>();
  boxRenderable->setMesh(boxMesh);
  boxRenderable->setMaterial(boxMat);
  HPhysicsMaterial boxPhysMat = PhysicsMaterial::create(1.0f, 1.0f, 0.5f);
  HBoxCollider boxCollider = box->addComponent<CBoxCollider>();
  boxCollider->setMaterial(boxPhysMat);
  boxCollider->setMass(25.0f);
  // HRigidbody boxRigidbody = box->addComponent<CRigidbody>();

  // Bunny
  // HTexture dragonAlbedo = Asset::loadTexture("res/textures/dragon.png");
  auto [bunnyMesh, bunnyPhysMesh] =
      Asset::loadMeshWithPhysics("res/models/stanford-bunny.fbx", 0.006f);
  HMaterial bunnyMat = Material::create(shader);
  bunnyMat->setTexture("gAlbedoTex", texGrid2);
  HSceneObject bunny = SceneObject::create("bunny");
  bunny->setPosition(Vector3(5, 3, 4));
  HRenderable bunnyRenderable = bunny->addComponent<CRenderable>();
  bunnyRenderable->setMesh(bunnyMesh);
  bunnyRenderable->setMaterial(bunnyMat);
  HPhysicsMaterial bunnyPhysMat = PhysicsMaterial::create(1.0f, 1.0f, 1.0f);
  HMeshCollider bunnyCollider = bunny->addComponent<CMeshCollider>();
  bunnyCollider->setMaterial(bunnyPhysMat);
  bunnyCollider->setMass(15.0f);
  bunnyCollider->setMesh(bunnyPhysMesh);
}

// -------------------------------------------------------------------------- //

void Editor::setupGUI() {
  using namespace bs;

  // Setup GUI
  HSceneObject gui = SceneObject::create("GUI");
  HCamera cameraComp = m_camera->getComponent<CCamera>();
  HGUIWidget guiComp = gui->addComponent<CGUIWidget>(cameraComp);
  GUIPanel *panel = guiComp->getPanel();

  static u32 count = 0;

  // Build interface
  GUILabel *label =
      panel->addNewElement<GUILabel>(HString("Press Escape to quit"));
  panel->setPosition(4, 4);

  GUIButton *quitButton = panel->addNewElement<GUIButton>(HString("Quit"));
  quitButton->setWidth(90);
  quitButton->setPosition(2, 18);
  quitButton->onClick.connect([]() { gApplication().quitRequested(); });

  GUIToggle *frameToggle = panel->addNewElement<GUIToggle>(HString("Frame"));
  frameToggle->setPosition(2, 40);
  frameToggle->onToggled.connect([this](bool t) {
    DebugDraw::instance().clear();
    m_obstrField->debugDraw(Vector3(), t);
  });

  GUIToggle *debugToggle =
      panel->addNewElement<GUIToggle>(HString("Obstruction Toggle"));
  debugToggle->setPosition(20, 40);
  debugToggle->onToggled.connect([this](bool t) {
    DebugDraw::instance().clear();
    if (t) {
      m_obstrField->debugDraw(Vector3(), true);
    }
  });
}

// -------------------------------------------------------------------------- //

void Editor::registerControls() {
  using namespace bs;

  auto inputConfig = gVirtualInput().getConfiguration();

  // Camera controls for buttons
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
  inputConfig->registerButton("Up", BC_SPACE);

  // Camera controls
  inputConfig->registerAxis("Horizontal",
                            VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
  inputConfig->registerAxis("Vertical",
                            VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));
}

} // namespace wind
