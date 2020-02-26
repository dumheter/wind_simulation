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
#include "shared/log.hpp"
#include "shared/scene/builder.hpp"

#include <filesystem>

#include <Components/BsCCamera.h>
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
#include <Resources/BsResourceManifest.h>
#include <Resources/BsResources.h>
#include <Scene/BsPrefab.h>
#include <Scene/BsSceneManager.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// UI Implementation
// ========================================================================== //

namespace wind {

UI::UI(Editor *editor) : m_editor(editor) {
  using namespace bs;

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
      m_debugFieldKind = static_cast<WindSimulation::FieldKind>(idx);
      DebugDraw::instance().clear();
      if (m_debugDraw) {
        m_editor->getSim()->debugDraw(m_debugFieldKind, Vec3F(),
                                      m_debugDrawFrame);
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
        m_editor->getSim()->debugDraw(m_debugFieldKind, Vec3F(),
                                      m_debugDrawFrame);
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
        m_editor->getSim()->debugDraw(m_debugFieldKind, Vec3F(),
                                      m_debugDrawFrame);
      }
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
      if (t) {
        m_editor->simEnable();
      } else {
        m_editor->simDisable();
      }
      logVerbose("Simulation {}", t ? "running" : "stopped");
    });

    GUILabel *label1 = panel->addNewElement<GUILabel>(HString("(1.0)"));
    label1->setPosition(244, height);

    GUISliderHorz *slider = panel->addNewElement<GUISliderHorz>();
    slider->setPosition(142, height);
    slider->setWidth(100);
    slider->setRange(0.0, 10.0f);
    slider->setValue(1.0f);
    slider->onChanged.connect([this, label1](f32 value) {
      m_editor->setSimSpeed(value);
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
        m_editor->setSimNumSteps(num);
        m_editor->simEnable();
        m_runToggle->toggleOn();
        logVerbose("Starting to run simulation for {} steps", num);
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
        m_editor->getSim()->addDensitySource();
        logVerbose("Added density sources");
      });
    }

    {
      GUIButton *button = panel->addNewElement<GUIButton>(HString("Add Sink"));
      button->setWidth(90);
      button->setPosition(214, height);
      button->onClick.connect([this]() {
        m_editor->getSim()->addDensitySink();
        logVerbose("Added density sinks");
      });
    }

    GUIButton *button = panel->addNewElement<GUIButton>(HString("Clear"));
    button->setWidth(90);
    button->setPosition(308, height);
    button->onClick.connect([this]() {
      DensityField *d = m_editor->getSim()->D();
      DensityField *d0 = m_editor->getSim()->D0();
      for (u32 i = 0; i < d->getDataSize(); i++) {
        d->get(i) = 0;
        d0->get(i) = 0;
      }
      logVerbose("Cleared density");
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
        m_editor->getSim()->addVelocitySource();
        logVerbose("Added velocity sources");
      });
    }

    {
      GUIButton *button = panel->addNewElement<GUIButton>(HString("Add Sink"));
      button->setWidth(90);
      button->setPosition(214, height);
      button->onClick.connect([this]() {
        m_editor->getSim()->addVelocitySink();
        logVerbose("Added velocity sinks");
      });
    }

    GUIButton *button = panel->addNewElement<GUIButton>(HString("Clear"));
    button->setWidth(90);
    button->setPosition(308, height);
    button->onClick.connect([this]() {
      VectorField *v = m_editor->getSim()->V();
      VectorField *v0 = m_editor->getSim()->V0();
      for (u32 i = 0; i < v->getDataSize(); i++) {
        v->set(i, Vec3F(0.4f, 0.05f, 0.3f));
        v0->set(i, Vec3F(0.4f, 0.05f, 0.3f));
      }
      logVerbose("Cleared velocity");
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
        m_editor->getSim()->setDensityDiffusionActive(t);
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
      m_editor->getSim()->setDensityAdvectionActive(t);
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
        m_editor->getSim()->setVelocityDiffusionActive(t);
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
      m_editor->getSim()->setVelocityAdvectionActive(t);
      logVerbose("Velocity advection: {}", t ? "ENABLED" : "DISABLED");
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
          "scene_default", Editor::GROUND_PLANE_SCALE));
      logVerbose("loading default scene");
    });

    height += button->getBounds().height + 2;
  }

  // Save/Load scene
  {
    GUIInputBox *input = panel->addNewElement<GUIInputBox>();
    input->setWidth(100);
    input->setPosition(4, height);

    GUIButton *bSave = panel->addNewElement<GUIButton>(HString("Save"));
    bSave->setWidth(30);
    bSave->setPosition(106, height);
    bSave->onClick.connect([this, input]() {
      const bs::String path = input->getText() + ".json";
      logVerbose("saving scene ({})", path.c_str());
    });

    GUIButton *bLoad = panel->addNewElement<GUIButton>(HString("Load"));
    bLoad->setWidth(30);
    bLoad->setPosition(138, height);
    bLoad->onClick.connect([this, input]() {
      m_scenePath = input->getText() + ".json";
      std::filesystem::path path = m_scenePath.c_str();
      if (std::filesystem::exists(path)) {
        logVerbose("loading scene ({})", m_scenePath.c_str());
        m_editor->setScene(SceneBuilder::fromFile(m_scenePath), true);
        m_sceneAutoReload = true;
      } else {
        logWarning("Scene requested to load does not exist \"{}\"",
                   m_scenePath.c_str());
        m_sceneAutoReload = false;
      }
    });

    height += input->getBounds().height + 2;
  }
}

// -------------------------------------------------------------------------- //

void UI::onFixedUpdate(f32 delta) {
  // Redraw debug overlay each frame
  if (m_debugDraw) {
    MICROPROFILE_SCOPEI("Sim", "draw", MP_ORANGE1);
    m_editor->getSim()->debugDraw(m_debugFieldKind, Vec3F(), m_debugDrawFrame);
  }

  // Check current file modification date
  if (!m_scenePath.empty() && m_sceneAutoReload) {
    std::filesystem::path path = m_scenePath.c_str();
    if (std::filesystem::exists(path)) {
      std::filesystem::file_time_type time =
          std::filesystem::last_write_time(path);
      if (time != m_sceneEditTime) {
        logVerbose("reloading scene ({})", m_scenePath.c_str());
        m_editor->setScene(SceneBuilder::fromFile(m_scenePath), true);
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