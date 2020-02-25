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
#include "factory.hpp"
#include "log.hpp"
#include "microprofile/microprofile.h"
#include "ui.hpp"
#include "utility/util.hpp"

#include <alflib/core/assert.hpp>

#include <BsCameraFlyer.h>
#include <Components/BsCBoxCollider.h>
#include <Components/BsCCamera.h>
#include <Components/BsCMeshCollider.h>
#include <Components/BsCPlaneCollider.h>
#include <Components/BsCRenderable.h>
#include <Components/BsCRigidbody.h>
#include <Components/BsCSkybox.h>
#include <Debug/BsDebugDraw.h>
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

Editor::Editor()
    : App(App::MakeInfo("Editor", WINDOW_WIDTH, WINDOW_HEIGHT, 20)) {}

// -------------------------------------------------------------------------- //

Editor::~Editor() { delete m_ui; }

// -------------------------------------------------------------------------- //

void Editor::onStartup() {
  using namespace bs;

  const SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
  const RenderWindowProperties &windowProp = window->getProperties();

  // Register controls
  registerControls();

  // Setup root
  m_root = SceneObject::create("root");

  // Create camera
  m_camera = SceneObject::create("camera");
  m_camera->setParent(m_root);
  m_camera->setPosition(
      Vec3F(GROUND_PLANE_SCALE, 2.5f, GROUND_PLANE_SCALE - 4.0f) * 0.65f);
  m_camera->lookAt(Vec3F(.0f, 1.5f, .0f));
  HCamera cameraComp = m_camera->addComponent<CCamera>();
  cameraComp->getViewport()->setTarget(window);
  cameraComp->setNearClipDistance(0.005f);
  cameraComp->setFarClipDistance(1000);
  cameraComp->setAspectRatio(windowProp.width / f32(windowProp.height));
  const SPtr<RenderSettings> renderSettings = cameraComp->getRenderSettings();
  renderSettings->enableIndirectLighting = true;
  cameraComp->setRenderSettings(renderSettings);
  const HCameraFlyer cameraFlyerComp = m_camera->addComponent<CameraFlyer>();

  // Setup UI
  m_ui = new UI(this);

  // Setup default scene
  // setScene(EditorFactory::createEmptyScene("scene_empty",
  // GROUND_PLANE_SCALE));

  // Create simulation
  m_windSim = new WindSimulation(u32(GROUND_PLANE_SCALE * 2.0f), 6,
                                 u32(GROUND_PLANE_SCALE * 2.0f), 0.25f);
  m_windSim->buildForScene(SceneManager::instance().getMainScene());
}

// -------------------------------------------------------------------------- //

void Editor::onPreUpdate(f32 delta) {
  using namespace bs;

  // Quit 'escape' or 'q'
  if (gInput().isButtonDown(ButtonCode::BC_ESCAPE) ||
      gInput().isButtonDown(ButtonCode::BC_Q)) {
    gApplication().quitRequested();
  }

  // Full-screen toggle 'F'
  if (gInput().isButtonDown(ButtonCode::BC_F1)) {
    if (isFullscreen()) {
      exitFullscreen();
    } else {
      enterFullscreen(App::VideoMode{0, 0}, 0);
    }
  } else if (gInput().isButtonDown(ButtonCode::BC_F2)) {
    if (isFullscreen()) {
      exitFullscreen();
    } else {
      enterFullscreen(App::VideoMode{0, 0}, 1);
    }
  }

  // Add density source 'X'
  if (gInput().isButtonHeld(ButtonCode::BC_X)) {
    m_windSim->addDensitySource();
    logVerbose("Added density sources");
  }
  // Add density sink 'Z'
  if (gInput().isButtonHeld(ButtonCode::BC_Z)) {
    m_windSim->addDensitySink();
    logVerbose("Added density sinks");
  }

  // Add velocity source 'V'
  if (gInput().isButtonHeld(ButtonCode::BC_V)) {
    m_windSim->addVelocitySource();
    logVerbose("Added velocity sources");
  }
  // Add velocity sink 'C'
  if (gInput().isButtonHeld(ButtonCode::BC_C)) {
    m_windSim->addVelocitySink();
    logVerbose("Added velocity sinks");
  }
}

// -------------------------------------------------------------------------- //

void Editor::onFixedUpdate(f32 delta) {
  using namespace bs;

  // Run simulation step
  if (m_runSim) {
    MICROPROFILE_SCOPEI("Sim", "step", MP_ORANGE1);
    if (m_simSteps == 1) {
      m_runSim = false;
      m_ui->setRunToggle(false);
      logVerbose("Finished simulating steps");
    }
    m_simSteps--;
    m_windSim->step(delta * m_simSpeed);
  }

  // Update UI
  m_ui->onFixedUpdate(delta);
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
  inputConfig->registerButton("Up", BC_SPACE);
  inputConfig->registerButton("Down", BC_LCONTROL);
  inputConfig->registerButton("FastMove", BC_LSHIFT);
  inputConfig->registerButton("RotateObj", BC_MOUSE_LEFT);
  inputConfig->registerButton("RotateCam", BC_MOUSE_RIGHT);

  // Camera controls
  inputConfig->registerAxis("Horizontal",
                            VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
  inputConfig->registerAxis("Vertical",
                            VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));
}

// -------------------------------------------------------------------------- //

void Editor::setScene(const bs::HSceneObject &scene) {
  // Set previous scene as not current
  if (m_scene != nullptr) {
    m_scene->setActive(false);
  }

  // Set new scene as current
  scene->setParent(m_root);
  m_scene = scene;
  m_scene->setActive(true);

  // Recreate simulation
  delete m_windSim;
  m_windSim = new WindSimulation(u32(GROUND_PLANE_SCALE * 2.0f), 6,
                                 u32(GROUND_PLANE_SCALE * 2.0f), 0.25f);
  m_windSim->buildForScene(bs::SceneManager::instance().getMainScene());

  // Debug dump
  logInfo("Dumping scene structure");
  Util::dumpScene(m_root);
}

} // namespace wind
