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

#include "shared/sim/wind_sim.hpp"
#include "shared/types.hpp"

#include <filesystem>

#include <GUI/BsGUIToggle.h>

// ========================================================================== //
// UI Declaration
// ========================================================================== //

namespace wind {

class Editor;

///	Class that represents the Editor UI
class UI {
public:
  /// Construct the UI with the editor for which it should be displayed
  explicit UI(Editor *editor);

  /// Update UI
  void onFixedUpdate(f32 delta);

  /// Updates the toggle status of the run button
  void setRunToggle(bool toggled);

private:
  /// Pointer to the editor instance
  Editor *m_editor;

  /// Run toggle button
  bs::GUIToggle *m_runToggle;

  /// Current scene path
  String m_scenePath = "";
  /// Whether scene auto-reload is enabled
  bool m_sceneAutoReload = false;
  /// Time that the scene file was last edited
  std::filesystem::file_time_type m_sceneEditTime;

  /// Debug type
  WindSimulation::FieldKind m_debugFieldKind =
      WindSimulation::FieldKind::DENSITY;
  /// Whether to draw debug data
  bool m_debugDraw = false;
  /// Whether to draw debug data frame
  bool m_debugDrawFrame = false;
};

} // namespace wind