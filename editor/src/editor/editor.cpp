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

#include "editor/factory.hpp"
#include "editor/ui.hpp"
#include "microprofile/microprofile.h"
#include "shared/asset.hpp"
#include "shared/camera_flyer.h"
#include "shared/scene/scene.hpp"
#include "shared/utility/util.hpp"

#include <dlog/dlog.hpp>
#include <Components/BsCCamera.h>
#include <Components/BsCRenderable.h>
#include <Components/BsCRigidbody.h>
#include <Debug/BsDebugDraw.h>
#include <Importer/BsImporter.h>
#include <Input/BsInput.h>
#include <Resources/BsBuiltinResources.h>
#include <Scene/BsSceneObject.h>

#include "shared/scene/builder.hpp"
#include "shared/scene/component/cpaint.hpp"
#include "shared/scene/component/csim.hpp"

// ========================================================================== //
// Editor Implementation
// ========================================================================== //

namespace wind {

Editor::Editor() : App(makeInfo("Editor", kWindowWidth, kWindowHeight, 20)) {}

// -------------------------------------------------------------------------- //

Editor::~Editor() { delete m_ui; }

// -------------------------------------------------------------------------- //

void Editor::onStartup() {
  const bs::SPtr<bs::RenderWindow> window =
      bs::gApplication().getPrimaryWindow();
  const bs::RenderWindowProperties &windowProp = window->getProperties();

  // Register controls
  registerControls();

  // Setup root
  m_root = bs::SceneObject::create("root");

  // Create camera
  m_camera = bs::SceneObject::create("camera");
  m_camera->setParent(m_root);
  m_camera->setPosition(
      Vec3F(kGroundPlaneScale, 2.5f, kGroundPlaneScale - 4.0f) * 0.65f);
  m_camera->lookAt(Vec3F(.0f, 1.5f, .0f));
  bs::HCamera cameraComp = m_camera->addComponent<bs::CCamera>();
  cameraComp->getViewport()->setTarget(window);
  cameraComp->setNearClipDistance(0.005f);
  cameraComp->setFarClipDistance(1000);
  cameraComp->setAspectRatio(windowProp.width / f32(windowProp.height));
  const bs::SPtr<bs::RenderSettings> renderSettings =
      cameraComp->getRenderSettings();
  renderSettings->enableIndirectLighting = true;
  cameraComp->setRenderSettings(renderSettings);
  const bs::HCameraFlyer cameraFlyerComp =
      m_camera->addComponent<bs::CameraFlyer>();

  // Setup UI
  m_ui = new UI(this);

  // Setup default scene
  setScene(Scene::loadFile(kDefaultSceneName));

  // TMP: Create sim object
  bs::HSceneObject simObj =
      ObjectBuilder(ObjectType::kEmpty).withName("wind_sim").build();
  simObj->setParent(getScene());
  HCSim sim = simObj->addComponent<CSim>();
  sim->build(u32(kGroundPlaneScale * 2.0f), 6, u32(kGroundPlaneScale * 2.0f),
             0.25f, bs::SceneManager::instance().getMainScene());

  // TMP: Save scene
  // Scene::saveFile("res/scenes/out.json", getScene());

  // TMP: Paint component
  // bs::HSceneObject obj =
  // getScene()->findPath("structures/house0/occluder"); if (obj) {
  //  HCPaint cpaint = obj->addComponent<CPaint>();
  //  cpaint->setPaintCallback([obj](Painter &painter) {
  //    //
  //    painter.setColor(Color::sRed);
  //    painter.drawArrow(Vec3F(0.0f, 1.0f, 0.0f), Vec3F(1.0f, 1.0f, 1.0f));
  //    painter.setColor(Color::sGreen);
  //    painter.drawArrow(Vec3F(2.0f, 1.0f, 2.0f), Vec3F(2.0f, 2.0f, 2.0f));
  //  });
  //}
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
    // m_windSim->addDensitySource();
    DLOG_VERBOSE("Added density sources");
  }
  // Add density sink 'Z'
  if (gInput().isButtonHeld(ButtonCode::BC_Z)) {
    // m_windSim->addDensitySink();
    DLOG_VERBOSE("Added density sinks");
  }

  // Add velocity source 'V'
  if (gInput().isButtonHeld(ButtonCode::BC_V)) {
    // m_windSim->addVelocitySource();
    DLOG_VERBOSE("Added velocity sources");
  }
  // Add velocity sink 'C'
  if (gInput().isButtonHeld(ButtonCode::BC_C)) {
    // m_windSim->addVelocitySink();
    DLOG_VERBOSE("Added velocity sinks");
  }
}

// -------------------------------------------------------------------------- //

void Editor::onFixedUpdate(f32 delta) {
  using namespace bs;
  m_ui->onFixedUpdate(delta);
}

// -------------------------------------------------------------------------- //

void Editor::onPaint(Painter &painter) {}

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

void Editor::setScene(const bs::HSceneObject &scene, bool destroy) {
  // Set previous scene as not current
  if (m_scene != nullptr) {
    m_scene->setActive(false);
    if (destroy) {
      m_scene->destroy();
    }
  }

  // Set new scene as current
  scene->setParent(m_root);
  m_scene = scene;
  m_scene->setActive(true);

  // Recreate simulation
  bs::HSceneObject simObj =
      ObjectBuilder(ObjectType::kEmpty).withName("wind_sim").build();
  simObj->setParent(getScene());
  HCSim sim = simObj->addComponent<CSim>();
  sim->build(u32(kGroundPlaneScale * 2.0f), 6, u32(kGroundPlaneScale * 2.0f),
             0.25f, bs::SceneManager::instance().getMainScene());
}

} // namespace wind
