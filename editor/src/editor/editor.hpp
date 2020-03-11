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

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "editor/sim/wind_sim.hpp"
#include "shared/app.hpp"

#include <Renderer/BsCamera.h>

// ========================================================================== //
// Editor Declaration
// ========================================================================== //

namespace wind {

class UI;

/// Main Editor class
class Editor final : public App {
public:
  /// Construct an editor
  Editor();

  /// Destruct editor
  ~Editor();

  /// \copydoc App::onStartup
  void onStartup() override;

  /// \copydoc App::onPreUpdate
  void onPreUpdate(f32 delta) override;

  /// \copydoc App::onFixedUpdate
  void onFixedUpdate(f32 delta) override;

  /// Enable running the simulation
  void simEnable() { m_runSim = true; }

  /// Disable running the simulation
  void simDisable() { m_runSim = true; }

  /// Set the simulation speed
  void setSimSpeed(f32 speed) { m_simSpeed = speed; }

  /// Set number of simulation steps to run for
  void setSimNumSteps(u32 numSteps) { m_simSteps = numSteps; }

  /// Returns a pointer to the wind simulation
  WindSimulation *getSim() const { return m_windSim; }

  /// Returns the root scene
  const bs::HSceneObject &getSceneRoot() { return m_root; }

  /// Returns the current scene
  const bs::HSceneObject &getScene() { return m_scene; }

  /// Set the current scene
  void setScene(const bs::HSceneObject &scene, bool destroy = false);

  /// Returns the main camera used to render the scenes in the editor.
  bs::HSceneObject getCamera() { return m_camera; }

private:
  /// Register controls
  void registerControls();

private:
  /// Editor UI
  UI *m_ui = nullptr;

  /// Whether to run simulation
  bool m_runSim = false;
  /// Speed at which simulation is running
  f32 m_simSpeed = 1.0f;
  /// Number of steps to the run the simulation for. With this set to 0 and the
  /// m_runSim flag set to true, the simulation will run continuously
  s32 m_simSteps = 0;

  /// Scene root
  bs::HSceneObject m_root;
  /// GUI camera
  // bs::HSceneObject m_guiCamera;
  /// Current scene
  bs::HSceneObject m_scene;
  /// Camera object
  bs::HSceneObject m_camera;

  /// Wind simulation field
  WindSimulation *m_windSim = nullptr;

public:
  /// Name of the default scene
  static constexpr char kDefaultSceneName[] = "res/scenes/world.json";

  /// Editor window width
  static constexpr u32 kWindowWidth = 1280;
  /// Editor window height
  static constexpr u32 kWindowHeight = 720;

  /// Scale of the ground plane
  static constexpr f32 kGroundPlaneScale = 4.0f;
};

} // namespace wind