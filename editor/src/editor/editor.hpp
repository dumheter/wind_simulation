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

#include "shared/app.hpp"
#include "shared/scene/component/csim.hpp"

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

  /// \copydoc App::onPaint
  void onPaint(Painter &painter) override;

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

  /// Scene root
  bs::HSceneObject m_root;
  /// GUI camera
  // bs::HSceneObject m_guiCamera;
  /// Current scene
  bs::HSceneObject m_scene;
  /// Camera object
  bs::HSceneObject m_camera;

public:
  /// Name of the default scene
  static constexpr char kDefaultSceneName[] = "res/scenes/sc.json";

  /// Editor window width
  static constexpr u32 kWindowWidth = 1280;
  /// Editor window height
  static constexpr u32 kWindowHeight = 720;

  /// Scale of the ground plane
  static constexpr f32 kGroundPlaneScale = 7.0f;
};

} // namespace wind
