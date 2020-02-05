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

#include "math/vector_field.hpp"
#include <BsCameraFlyer.h>
#include <Components/BsCCamera.h>
#include <Components/BsCRenderable.h>
#include <Components/BsCSkybox.h>
#include <Debug/BsDebugDraw.h>
#include <Importer/BsImporter.h>
#include <Importer/BsMeshImportOptions.h>
#include <Importer/BsTextureImportOptions.h>
#include <Material/BsMaterial.h>
#include <Mesh/BsMesh.h>
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
  VectorField *vf = new VectorField(30, 30, 30, 0.5f);
  vf->debugDraw(Vector3(), false);
}

// -------------------------------------------------------------------------- //

void Editor::OnPreUpdate() {}

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
  HSceneObject camera = SceneObject::create("Camera");
  camera->setPosition(Vector3(.0f, 2.5f, -4.0f) * 0.65f);
  camera->lookAt(Vector3(.0f, 1.5f, .0f));
  HCamera cameraComp = camera->addComponent<CCamera>();
  cameraComp->getViewport()->setTarget(window);
  cameraComp->setNearClipDistance(0.005f);
  cameraComp->setFarClipDistance(1000);
  cameraComp->setAspectRatio(windowProp.width / f32(windowProp.height));
  const SPtr<RenderSettings> renderSettings = cameraComp->getRenderSettings();
  renderSettings->enableIndirectLighting = true;
  cameraComp->setRenderSettings(renderSettings);

  // Flying camera
  const HCameraFlyer cameraFlyerComp = camera->addComponent<CameraFlyer>();
}

// -------------------------------------------------------------------------- //

void Editor::setupScene() {
  using namespace bs;

  // Load textures
  const HTexture texGrid = Asset::loadTexture("res/textures/grid.png");
  const HTexture texGrid2 = Asset::loadTexture("res/textures/grid_2.png");

  // Setup plane
  const HShader shader =
      gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
  HMaterial planeMat = Material::create(shader);
  planeMat->setTexture("gAlbedoTex", texGrid);
  planeMat->setVec2("gUVTile", Vector2::ONE * GROUND_PLANE_SCALE * 4.0f);
  const HMesh planeMesh = gBuiltinResources().getMesh(BuiltinMesh::Quad);

  HSceneObject plane = SceneObject::create("Plane");
  plane->setScale(Vector3(GROUND_PLANE_SCALE, 1.0f, GROUND_PLANE_SCALE));
  plane->setRotation(Quaternion(Degree(0), Degree(0), Degree(180)));
  HRenderable planeRenderable = plane->addComponent<CRenderable>();
  planeRenderable->setMesh(planeMesh);
  planeRenderable->setMaterial(planeMat);
}

// -------------------------------------------------------------------------- //

void Editor::setupGUI() {}

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
