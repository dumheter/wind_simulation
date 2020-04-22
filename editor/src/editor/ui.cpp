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

#include "ui.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "editor/editor.hpp"
#include "editor/factory.hpp"
#include "microprofile/microprofile.h"
#include "shared/scene/builder.hpp"
#include "shared/scene/scene.hpp"

#include <filesystem>

#include <Components/BsCCamera.h>
#include <GUI/BsCGUIWidget.h>
#include <GUI/BsGUIButton.h>
#include <GUI/BsGUIInputBox.h>
#include <GUI/BsGUILabel.h>
#include <GUI/BsGUILayout.h>
#include <GUI/BsGUIListBox.h>
#include <GUI/BsGUIPanel.h>
#include <GUI/BsGUISlider.h>
#include <GUI/BsGUIToggle.h>
#include <Resources/BsResources.h>
#include <Scene/BsSceneObject.h>
#include <dlog/dlog.hpp>

#include "shared/debug/debug_manager.hpp"

// ========================================================================== //
// UI Implementation
// ========================================================================== //

namespace wind {

UI::UI(Editor *editor) : m_editor(editor) {
  using namespace bs;

  m_scenePath = Editor::kDefaultSceneName;

  const SPtr<RenderWindow> &window = gApplication().getPrimaryWindow();
  const RenderWindowProperties &windowProp = window->getProperties();

  // Setup scene object
  HSceneObject gui = SceneObject::create("gui");
  gui->setParent(m_editor->getSceneRoot());

  // Setup GUI
  HCamera cameraComp = m_editor->getCamera()->getComponent<CCamera>();
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
      DebugManager::setS32(WindSimulation::kDebugFieldTypeKey, idx);
    });
    height += listBox->getBounds().height + 2;
    // TMP nicer default
    DebugManager::setS32(WindSimulation::kDebugFieldTypeKey, 1);
  }

  // Debug drawing
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Draw"));
    label->setPosition(4, height);

    GUIToggle *toggle = panel->addNewElement<GUIToggle>(HString("Debug Draw"));
    toggle->setPosition(120, height);
    // TMP nicer default
    DebugManager::setBool(WindSimulation::kDebugPaintKey, true);
    toggle->toggleOn();
    toggle->onToggled.connect([this](bool t) {
      DebugManager::setBool(WindSimulation::kDebugPaintKey, t);
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
      DebugManager::setBool(WindSimulation::kDebugPaintFrameKey, t);
    });

    height += toggle->getBounds().height + 2;
  }

  // Run simulation
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Run simulation"));
    label->setPosition(4, height);

    m_runToggle = panel->addNewElement<GUIToggle>(HString("DebugRunSim"));
    m_runToggle->setPosition(120, height);
    m_runToggle->onToggled.connect([this](bool t) {
      DebugManager::setBool(WindSimulation::kDebugRun, t);
    });

    GUILabel *label1 = panel->addNewElement<GUILabel>(HString("(1.0)"));
    label1->setPosition(244, height);

    GUISliderHorz *slider = panel->addNewElement<GUISliderHorz>();
    slider->setPosition(142, height);
    slider->setWidth(100);
    slider->setRange(0.0, 10.0f);
    slider->setValue(1.0f);
    slider->onChanged.connect([this, label1](f32 value) {
      DebugManager::setF32(WindSimulation::kDebugRunSpeed, value);
      std::stringstream sstr;
      sstr << "(" << std::fixed << std::setprecision(2) << value << ")";
      label1->setContent(GUIContent(HString(sstr.str().c_str())));
    });

    height += m_runToggle->getBounds().height + 2;
  }

  // Run simulation N steps
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Run N steps"));
    label->setPosition(4, height);

    GUIInputBox *input = panel->addNewElement<GUIInputBox>();
    input->setWidth(50);
    input->setPosition(120, height);
    input->setFilter([](const String &str) {
      for (char c : str) {
        if (!std::isdigit(c)) {
          return false;
        }
      }
      return true;
    });

    GUIButton *button = panel->addNewElement<GUIButton>(HString("Run"));
    button->setWidth(20);
    button->setPosition(172, height);
    button->onClick.connect([this, input]() {
      const String &s = input->getText();
      u32 num = std::stoul(s.c_str());
      if (num > 0) {
        // m_editor->setSimNumSteps(num);
        // m_editor->simEnable();
        // m_runToggle->toggleOn();
        // DLOG_VERBOSE("Starting to run simulation for {} steps", num);
      }
    });

    height += button->getBounds().height + 2;
  }

  // Density
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Density source"));
    label->setPosition(4, height);

    {
      GUIButton *button =
          panel->addNewElement<GUIButton>(HString("Add Source"));
      button->setWidth(90);
      button->setPosition(120, height);
      button->onClick.connect([this]() {
        // m_editor->getSim()->addDensitySource();
        // DLOG_VERBOSE("Added density sources");
      });
    }

    {
      GUIButton *button = panel->addNewElement<GUIButton>(HString("Add Sink"));
      button->setWidth(90);
      button->setPosition(214, height);
      button->onClick.connect([this]() {
        // m_editor->getSim()->addDensitySink();
        // DLOG_VERBOSE("Added density sinks");
      });
    }

    GUIButton *button = panel->addNewElement<GUIButton>(HString("Clear"));
    button->setWidth(90);
    button->setPosition(308, height);
    button->onClick.connect([this]() {
      // DensityField *d = m_editor->getSim()->D();
      // DensityField *d0 = m_editor->getSim()->D0();
      // for (u32 i = 0; i < d->getDataSize(); i++) {
      //  d->get(i) = 0;
      //  d0->get(i) = 0;
      //}
      // DLOG_VERBOSE("Cleared density");
    });

    height += button->getBounds().height + 2;
  }

  // Velocity
  {
    GUILabel *label =
        panel->addNewElement<GUILabel>(HString("Velocity source"));
    label->setPosition(4, height);

    {
      GUIButton *button =
          panel->addNewElement<GUIButton>(HString("Add Source"));
      button->setWidth(90);
      button->setPosition(120, height);
      button->onClick.connect([this]() {
        // m_editor->getSim()->addVelocitySource();
        // DLOG_VERBOSE("Added velocity sources");
      });
    }

    {
      GUIButton *button = panel->addNewElement<GUIButton>(HString("Add Sink"));
      button->setWidth(90);
      button->setPosition(214, height);
      button->onClick.connect([this]() {
        // m_editor->getSim()->addVelocitySink();
        // DLOG_VERBOSE("Added velocity sinks");
      });
    }

    GUIButton *button = panel->addNewElement<GUIButton>(HString("Clear"));
    button->setWidth(90);
    button->setPosition(308, height);
    button->onClick.connect([this]() {
      // VectorField *v = m_editor->getSim()->V();
      // VectorField *v0 = m_editor->getSim()->V0();
      // for (u32 i = 0; i < v->getDataSize(); i++) {
      //  v->set(i, Vec3F(0.4f, 0.05f, 0.3f));
      //  v0->set(i, Vec3F(0.4f, 0.05f, 0.3f));
      //}
      // DLOG_VERBOSE("Cleared velocity");
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
        // m_editor->getSim()->setDensityDiffusionActive(t);
        // DLOG_VERBOSE("Density diffusion: {}", t ? "ENABLED" : "DISABLED");
      });
      toggle->toggleOn();
    }

    GUILabel *l1 = panel->addNewElement<GUILabel>(HString("advection:"));
    l1->setPosition(150, height);
    GUIToggle *toggle =
        panel->addNewElement<GUIToggle>(HString("DebugSimDensAdv"));
    toggle->setPosition(220, height);
    toggle->onToggled.connect([this](bool t) {
      // m_editor->getSim()->setDensityAdvectionActive(t);
      // DLOG_VERBOSE("Density advection: {}", t ? "ENABLED" : "DISABLED");
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
        // m_editor->getSim()->setVelocityDiffusionActive(t);
        // DLOG_VERBOSE("Velocity diffusion: {}", t ? "ENABLED" : "DISABLED");
      });
      toggle->toggleOn();
    }

    GUILabel *label = panel->addNewElement<GUILabel>(HString("advection:"));
    label->setPosition(150, height);
    GUIToggle *toggle =
        panel->addNewElement<GUIToggle>(HString("DebugSimVelAdv"));
    toggle->setPosition(220, height);
    toggle->onToggled.connect([this](bool t) {
      // m_editor->getSim()->setVelocityAdvectionActive(t);
      // DLOG_VERBOSE("Velocity advection: {}", t ? "ENABLED" : "DISABLED");
    });
    toggle->toggleOn();

    height += toggle->getBounds().height + 2;
  }

  // Default scene
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Scene"));
    label->setPosition(4, height);

    GUIButton *button = panel->addNewElement<GUIButton>(HString("Default"));
    button->setWidth(90);
    button->setPosition(120, height);
    button->onClick.connect([this]() {
      m_editor->setScene(EditorFactory::createDefaultScene(
          "scene_default", Editor::kGroundPlaneScale));
      // DLOG_VERBOSE("loading default scene");
    });

    height += button->getBounds().height + 2;
  }

  // Save/Load scene
  {
    GUIInputBox *input = panel->addNewElement<GUIInputBox>();
    input->setText(m_scenePath);
    input->setWidth(150);
    input->setPosition(4, height);

    GUIButton *bSave = panel->addNewElement<GUIButton>(HString("Save"));
    bSave->setWidth(30);
    bSave->setPosition(156, height);
    bSave->onClick.connect([this, input]() {
      std::filesystem::path path = input->getText().c_str();
      // DLOG_VERBOSE("saving scene ({})", path);
      String _path = path.string().c_str();
      m_scenePath = path.string().c_str();
      m_sceneAutoReload = false;
      Scene::saveFile(_path, m_editor->getScene());
    });

    GUIButton *bLoad = panel->addNewElement<GUIButton>(HString("Load"));
    bLoad->setWidth(30);
    bLoad->setPosition(188, height);
    bLoad->onClick.connect([this, input]() {
      std::filesystem::path path = input->getText().c_str();
      if (std::filesystem::exists(path)) {
        // DLOG_VERBOSE("loading scene ({})", path);
        m_scenePath = path.string().c_str();
        m_editor->setScene(Scene::loadFile(m_scenePath), true);
        m_sceneAutoReload = true;
      } else {
        DLOG_WARNING("Scene requested to load does not exist \"{}\"",
                     path.string());
        m_sceneAutoReload = false;
      }
    });

    height += input->getBounds().height + 2;
  }

  // Delta field
  {

    GUILabel *label = panel->addNewElement<GUILabel>(HString("Delta:"));
    label->setPosition(4, height);

    {
      GUIToggle *toggle =
          panel->addNewElement<GUIToggle>(HString("DeltaDelta"));
      toggle->setPosition(120, height);
      toggle->onToggled.connect(
          [this](bool t) { m_editor->m_deltaField.setDrawDelta(t); });
      toggle->toggleOff();
    }

    {
      GUIToggle *toggle = panel->addNewElement<GUIToggle>(HString("DeltaSim"));
      toggle->setPosition(140, height);
      toggle->onToggled.connect(
          [this](bool t) { m_editor->m_deltaField.setDrawSim(t); });
      toggle->toggleOff();
    }

    GUIToggle *toggle = panel->addNewElement<GUIToggle>(HString("DeltaBake"));
    toggle->setPosition(160, height);
    toggle->onToggled.connect(
        [this](bool t) { m_editor->m_deltaField.setDrawBaked(t); });
    toggle->toggleOff();

    height += toggle->getBounds().height + 2;
  }

  // Splines
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Splines:"));
    label->setPosition(4, height);

    GUIToggle *toggle =
        panel->addNewElement<GUIToggle>(HString("SplineDebugToggle"));
    toggle->setPosition(120, height);
    toggle->onToggled.connect(
        [this](bool t) { DebugManager::setBool("debug_draw_spline", t); });
    toggle->toggleOff();

    height += toggle->getBounds().height + 2;
  }

  // Bake
  {
    GUIButton *btn = panel->addNewElement<GUIButton>(HString("Bake"));
    btn->setWidth(70);
    btn->setPosition(120, height);
    btn->onClick.connect([this]() { m_editor->bake(); });
    height += btn->getBounds().height + 2;
  }

  // Camera
  {
    GUILabel *label =
        panel->addNewElement<GUILabel>(HString("Toggle Camera Mode:"));
    label->setPosition(4, height);

    GUIToggle *toggle =
        panel->addNewElement<GUIToggle>(HString("CameraModeToggle"));
    toggle->setPosition(120, height);
    toggle->onToggled.connect([this](bool ortho) {
      if (ortho) {
        m_editor->m_cameraComp->setProjectionType(
            bs::ProjectionType::PT_ORTHOGRAPHIC);
        auto primaryWindow = bs::gApplication().getPrimaryWindow();
        auto &windowProps = primaryWindow->getProperties();
        constexpr f32 kZoom = 30;
        m_editor->m_cameraComp->setOrthoWindow(windowProps.width / kZoom,
                                               windowProps.height / kZoom);
      } else {
        m_editor->m_cameraComp->setProjectionType(
            bs::ProjectionType::PT_PERSPECTIVE);
      }
    });
    toggle->toggleOff();

    height += toggle->getBounds().height + 2;
  }

  // Ortho zoom
  {
    GUILabel *label = panel->addNewElement<GUILabel>(HString("Ortho zoom"));
    label->setPosition(4, height);

    GUISliderHorz *slider = panel->addNewElement<GUISliderHorz>();
    slider->setPosition(120, height);
    slider->setWidth(100);
    slider->setRange(0.0f, 1.0f);
    slider->setValue(0.1f);
    slider->onChanged.connect([this](f32 value) {
      const f32 zoom = 300 * value;
      auto primaryWindow = bs::gApplication().getPrimaryWindow();
      auto &windowProps = primaryWindow->getProperties();
      m_editor->m_cameraComp->setOrthoWindow(windowProps.width / zoom,
                                             windowProps.height / zoom);
    });

    height += m_runToggle->getBounds().height + 2;
  }

  // Toggle Wind Volumes
  {
    GUILabel *label =
        panel->addNewElement<GUILabel>(HString("Render Wind Volume:"));
    label->setPosition(4, height);

    GUIToggle *toggle =
        panel->addNewElement<GUIToggle>(HString("WindVolumeToggle"));
    toggle->setPosition(120, height);
    toggle->toggleOn();
    toggle->onToggled.connect([this](bool enable) {
      bs::Vector<bs::HSceneObject> windVolumes =
          m_editor->m_root->findChildren("WindVolumeVisual");
      if (windVolumes.empty()) {
        return;
      }
      for (auto &so : windVolumes) {
        so->setActive(enable);
      }
    });
  }

  // Save Bake
  // {
  //   GUIButton *btn = panel->addNewElement<GUIButton>(HString("Save Bake"));
  //   btn->setWidth(70);
  //   btn->setPosition(138, height);
  //   btn->onClick.connect([this]() { bake(); });
  //   height += btn->getBounds().height + 2;
  // }
}

// -------------------------------------------------------------------------- //

void UI::onFixedUpdate(f32 delta) {
  // Check current file modification date
  if (!m_scenePath.empty() && m_sceneAutoReload) {
    std::filesystem::path path = m_scenePath.c_str();
    if (std::filesystem::exists(path)) {
      std::filesystem::file_time_type time =
          std::filesystem::last_write_time(path);
      if (time != m_sceneEditTime) {
        // DLOG_VERBOSE("reloading scene ({})", m_scenePath.c_str());
        m_editor->setScene(Scene::loadFile(m_scenePath), true);
      }
      m_sceneEditTime = time;
    }
  }
}

// -------------------------------------------------------------------------- //

void UI::setRunToggle(bool toggled) {
  if (toggled) {
    m_runToggle->toggleOn();
  } else {
    m_runToggle->toggleOff();
  }
}

} // namespace wind
