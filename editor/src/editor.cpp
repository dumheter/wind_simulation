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
#include "log.hpp"

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
#include <GUI/BsGUIListBox.h>
#include <GUI/BsGUIPanel.h>
#include <GUI/BsGUIProgressBar.h>
#include <GUI/BsGUISlider.h>
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

Editor::Editor() : App(Info{"Editor", WINDOW_WIDTH, WINDOW_HEIGHT}) {}

// -------------------------------------------------------------------------- //

void Editor::onStartup() {
  using namespace bs;

  // Setup editor
  registerControls();
  setupCamera();
  setupScene();
  setupGUI();

  // Create simulation
  m_windSim = new WindSimulation(u32(GROUND_PLANE_SCALE * 2.0f), 6,
                                 u32(GROUND_PLANE_SCALE * 2.0f), 0.5f);
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
  if (gInput().isButtonDown(ButtonCode::BC_F)) {
    if (isFullscreen()) {
      exitFullscreen();
    } else {
      enterFullscreen();
    }
  }

  // Add source 'X'
  if (gInput().isButtonHeld(ButtonCode::BC_X)) {
    m_windSim->addDensitySource();
    logVerbose("Added density sources");
  }
  // Add sink 'Z'
  if (gInput().isButtonHeld(ButtonCode::BC_Z)) {
    m_windSim->addDensitySink();
    logVerbose("Added density sinks");
  }
}

// -------------------------------------------------------------------------- //

void Editor::onFixedUpdate(f32 delta) {
  using namespace bs;

  // Run simulation step
  if (m_runSim) {
    m_windSim->step(delta);
  }

  // Redraw each frame
  if (m_debugDraw) {
    m_windSim->debugDraw(m_debugFieldKind, Vector3(), m_debugDrawFrame);
  }
} // namespace wind

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

  // Root geometry node
  m_geometry = SceneObject::create("Geometry");

#if 1
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

  HSceneObject plane = m_geometry->create("Plane");
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
  HSceneObject box = m_geometry->create("Box");
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
  auto [bunnyMesh, bunnyPhysMesh] =
      Asset::loadMeshWithPhysics("res/models/stanford-bunny.fbx", 0.006f);
  HMaterial bunnyMat = Material::create(shader);
  bunnyMat->setTexture("gAlbedoTex", texGrid2);
  HSceneObject bunny = m_geometry->create("Bunny");
  bunny->setPosition(Vector3(5, 3, 4));
  HRenderable bunnyRenderable = bunny->addComponent<CRenderable>();
  bunnyRenderable->setMesh(bunnyMesh);
  bunnyRenderable->setMaterial(bunnyMat);
  HPhysicsMaterial bunnyPhysMat = PhysicsMaterial::create(1.0f, 1.0f, 1.0f);
  HMeshCollider bunnyCollider = bunny->addComponent<CMeshCollider>();
  bunnyCollider->setMaterial(bunnyPhysMat);
  bunnyCollider->setMass(15.0f);
  bunnyCollider->setMesh(bunnyPhysMesh);
#endif
}

// -------------------------------------------------------------------------- //

void Editor::setupGUI() {
  using namespace bs;

  // Setup GUI
  HSceneObject gui = SceneObject::create("GUI");
  HCamera cameraComp = m_camera->getComponent<CCamera>();
  HGUIWidget guiComp = gui->addComponent<CGUIWidget>(cameraComp);
  GUIPanel *panel = guiComp->getPanel();

  u32 height = 4;

  // Quitting
  {
    GUILabel *label = panel->addNewElement<GUILabel>(
        HString("Press 'Escape' or 'Q' to quit"));
    panel->setPosition(6, height);
    height += label->getBounds().height + 2;

    // Quit button
    GUIButton *button = panel->addNewElement<GUIButton>(HString("Quit"));
    button->setWidth(90);
    button->setPosition(4, height);
    button->onClick.connect([]() { gApplication().quitRequested(); });
    height += button->getBounds().height + 2;
  }

  // Fullscreen toggle
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Fullscreen"));
    label->setPosition(4, height);

    // Monitor selection
    Vector<HString> opts = {HString("Primary"), HString("Secondary"),
                            HString("Tertiary")};
    GUIListBox *listBox = panel->addNewElement<GUIListBox>(opts);
    listBox->setPosition(136, height);
    listBox->setWidth(100);

    GUIToggle *toggle =
        panel->addNewElement<GUIToggle>(HString("FullscreenToggle"));
    toggle->setPosition(120, height);
    toggle->onToggled.connect([this, listBox](bool t) {
      if (t) {
        auto selection = listBox->getElementStates();
        u32 idx = 0;
        for (u32 i = 0; i < selection.size(); i++) {
          if (selection[i]) {
            idx = i;
          }
        }
        enterFullscreen(VideoMode(), idx);
      } else {
        exitFullscreen();
      }
    });

    height += listBox->getBounds().height + 2;
  }

  // Scene enabling
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Draw scene"));
    label->setPosition(4, height);

    GUIToggle *toggle = panel->addNewElement<GUIToggle>(HString("DrawScene"));
    toggle->setPosition(120, height);
    toggle->onToggled.connect([this](bool t) {
      m_drawScene = t;
      m_geometry->setActive(m_drawScene);
    });

    height += toggle->getBounds().height + 2;
  }

  // Debug draw type
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Field type"));
    label->setPosition(4, height);

    Vector<HString> opts = {HString("Density"), HString("Vector"),
                            HString("Obstruction")};
    GUIListBox *listBox = panel->addNewElement<GUIListBox>(opts);
    listBox->setPosition(120, height);
    listBox->setWidth(100);
    listBox->onSelectionToggled.connect([=](UINT32 idx, bool _b) {
      m_debugFieldKind = static_cast<WindSimulation::FieldKind>(idx);
      DebugDraw::instance().clear();
      if (m_debugDraw) {
        m_windSim->debugDraw(m_debugFieldKind, Vector3(), m_debugDrawFrame);
      }
    });
    height += listBox->getBounds().height + 2;
  }

  // Debug drawing
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Draw"));
    label->setPosition(4, height);

    GUIToggle *toggle = panel->addNewElement<GUIToggle>(HString("Debug Draw"));
    toggle->setPosition(120, height);
    toggle->onToggled.connect([this](bool t) {
      DebugDraw::instance().clear();
      m_debugDraw = t;
      if (m_debugDraw) {
        m_windSim->debugDraw(m_debugFieldKind, Vector3(), m_debugDrawFrame);
      }
    });

    height += toggle->getBounds().height + 2;
  }

  // Debug frame drawing
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Draw frames"));
    label->setPosition(4, height);

    GUIToggle *toggle =
        panel->addNewElement<GUIToggle>(HString("DebugDrawFrame"));
    toggle->setPosition(120, height);
    toggle->onToggled.connect([this](bool t) {
      DebugDraw::instance().clear();
      m_debugDrawFrame = t;
      if (m_debugDraw) {
        m_windSim->debugDraw(m_debugFieldKind, Vector3(), m_debugDrawFrame);
      }
    });

    height += toggle->getBounds().height + 2;
  }

  // Debug vector direction
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Vectors"));
    label->setPosition(4, height);

    GUISliderHorz *xSlider = panel->addNewElement<GUISliderHorz>();
    xSlider->setPosition(120, height);
    xSlider->setWidth(50);
    xSlider->setRange(-1.0, 1.0f);
    xSlider->setValue(0.0f);
    xSlider->onChanged.connect([this](f32 value) {
      // bs::Vector3 vec = m_windSim->getVelocityField()->get(1, 1, 1);
      // vec.x = value;
      // for (u32 i = 0; i < m_windSim->getVelocityField()->getDataSize(); i++)
      // {
      //  m_windSim->getVelocityField()->get(i) = vec;
      //  m_windSim->getVelocityFieldPrev()->get(i) = vec;
      //}
    });

    GUISliderHorz *ySlider = panel->addNewElement<GUISliderHorz>();
    ySlider->setPosition(172, height);
    ySlider->setWidth(50);
    ySlider->setRange(-1.0, 1.0f);
    ySlider->setValue(0.0f);
    ySlider->onChanged.connect([this](f32 value) {
      // bs::Vector3 vec = m_windSim->getVelocityField()->get(1, 1, 1);
      // vec.y = value;
      // for (u32 i = 0; i < m_windSim->getVelocityField()->getDataSize(); i++)
      // {
      //  m_windSim->getVelocityField()->get(i) = vec;
      //  m_windSim->getVelocityFieldPrev()->get(i) = vec;
      //}
    });

    GUISliderHorz *zSlider = panel->addNewElement<GUISliderHorz>();
    zSlider->setPosition(224, height);
    zSlider->setWidth(50);
    zSlider->setRange(-1.0, 1.0f);
    zSlider->setValue(0.0f);
    zSlider->onChanged.connect([this](f32 value) {
      // bs::Vector3 vec = m_windSim->getVelocityField()->get(1, 1, 1);
      // vec.z = value;
      // for (u32 i = 0; i < m_windSim->getVelocityField()->getDataSize(); i++)
      // {
      //  m_windSim->getVelocityField()->get(i) = vec;
      //  m_windSim->getVelocityFieldPrev()->get(i) = vec;
      //}
    });

    height += xSlider->getBounds().height + 2;
  }

  // Debug run simulation
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Run simulation"));
    label->setPosition(4, height);

    GUIToggle *toggle = panel->addNewElement<GUIToggle>(HString("DebugRunSim"));
    toggle->setPosition(120, height);
    toggle->onToggled.connect([this](bool t) {
      m_runSim = t;
      logVerbose("Simulation {}", t ? "running" : "stopped");
    });

    height += toggle->getBounds().height + 2;
  }

  // Density source
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Density source"));
    label->setPosition(4, height);

    {
      GUIButton *button =
          panel->addNewElement<GUIButton>(HString("Add Source"));
      button->setWidth(90);
      button->setPosition(120, height);
      button->onClick.connect([this]() {
        m_windSim->addDensitySource();
        logVerbose("Added density sources");
      });
    }

    {
      GUIButton *button = panel->addNewElement<GUIButton>(HString("Add Sink"));
      button->setWidth(90);
      button->setPosition(214, height);
      button->onClick.connect([this]() {
        m_windSim->addDensitySink();
        logVerbose("Added density sinks");
      });
    }

    GUIButton *button = panel->addNewElement<GUIButton>(HString("Clear"));
    button->setWidth(90);
    button->setPosition(308, height);
    button->onClick.connect([this]() {
      DensityField *d = m_windSim->D();
      DensityField *d0 = m_windSim->D0();
      for (u32 i = 0; i < d->getDataSize(); i++) {
        d->get(i) = 0;
        d0->get(i) = 0;
      }
      logVerbose("Cleared density");
    });

    height += button->getBounds().height + 2;
  }

  // Sim density parts
  {
    {
      GUILabel *label =
          panel->addNewElement<GUILabel>(HString("Density diffusion:"));
      label->setPosition(4, height);
      GUIToggle *toggle =
          panel->addNewElement<GUIToggle>(HString("DebugSimDensDif"));
      toggle->setPosition(120, height);
      toggle->onToggled.connect([this](bool t) {
        m_windSim->setDensityDiffusionActive(t);
        logVerbose("Density diffusion: {}", t ? "ENABLED" : "DISABLED");
      });
      toggle->toggleOn();
    }

    GUILabel *l1 = panel->addNewElement<GUILabel>(HString("advection:"));
    l1->setPosition(150, height);
    GUIToggle *toggle =
        panel->addNewElement<GUIToggle>(HString("DebugSimDensAdv"));
    toggle->setPosition(220, height);
    toggle->onToggled.connect([this](bool t) {
      m_windSim->setDensityAdvectionActive(t);
      logVerbose("Density advection: {}", t ? "ENABLED" : "DISABLED");
    });
    toggle->toggleOn();
    height += toggle->getBounds().height + 2;
  }

  // Sim velocity parts
  {
    {
      GUILabel *label =
          panel->addNewElement<GUILabel>(HString("Velocity diffusion:"));
      label->setPosition(4, height);
      GUIToggle *toggle =
          panel->addNewElement<GUIToggle>(HString("DebugSimVelDif"));
      toggle->setPosition(120, height);
      toggle->onToggled.connect([this](bool t) {
        m_windSim->setVelocityDiffusionActive(t);
        logVerbose("Velocity diffusion: {}", t ? "ENABLED" : "DISABLED");
      });
      toggle->toggleOn();
    }

    GUILabel *label = panel->addNewElement<GUILabel>(HString("advection:"));
    label->setPosition(150, height);
    GUIToggle *toggle =
        panel->addNewElement<GUIToggle>(HString("DebugSimVelAdv"));
    toggle->setPosition(220, height);
    toggle->onToggled.connect([this](bool t) {
      m_windSim->setVelocityAdvectionActive(t);
      logVerbose("Velocity advection: {}", t ? "ENABLED" : "DISABLED");
    });
    toggle->toggleOn();

    height += toggle->getBounds().height + 2;
  }
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

} // namespace wind
