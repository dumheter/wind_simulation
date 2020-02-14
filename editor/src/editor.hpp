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

#include "app.hpp"
#include "sim/wind_sim.hpp"

#include <BsPrerequisites.h>
#include <GUI/BsGUIToggle.h>
#include <Renderer/BsCamera.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// Editor Declaration
// ========================================================================== //

namespace wind {

/* Main Editor class */
class Editor final : public App {
public:
  /* Construct an editor */
  Editor();

  /* \copydoc App::onStartup */
  void onStartup() override;

  /* \copydoc App::onPreUpdate */
  void onPreUpdate(f32 delta) override;

  /* \copydoc App::onFixedUpdate */
  void onFixedUpdate(f32 delta) override;

  void onTick() override;

private:
  /* Setup the camera */
  void setupCamera();

  /* Setup the editor scene */
  void setupScene();

  /* Setup the editor GUI */
  void setupGUI();

  /* Register controls */
  void registerControls();

private:
  /* Whether to draw scene */
  bool m_drawScene = true;

  /* Whether to run simulation */
  bool m_runSim = false;
  /// Speed at which simulation is running
  f32 m_simSpeed = 1.0f;
  /// Number of steps to the run the simulation for. With this set to 0 and the
  /// m_runSim flag set to true, the simulation will run continuously
  s32 m_simSteps = 0;

  /* Debug type */
  WindSimulation::FieldKind m_debugFieldKind =
      WindSimulation::FieldKind::DENSITY;
  /* Whether to draw debug data */
  bool m_debugDraw = false;
  /* Whether to draw debug data frame */
  bool m_debugDrawFrame = false;

  /* Camera object */
  bs::HSceneObject m_camera;

  /* Root geometry node */
  bs::HSceneObject m_geometry;

  /// Run toggle button
  bs::GUIToggle *m_runToggle;

  /* Wind simulation field */
  WindSimulation *m_windSim;

public:
  /* Editor window width */
  static constexpr u32 WINDOW_WIDTH = 1280;
  /* Editor window height */
  static constexpr u32 WINDOW_HEIGHT = 720;

  /* Scale of the ground plane */
  static constexpr f32 GROUND_PLANE_SCALE = 4.0f;
};

} // namespace wind