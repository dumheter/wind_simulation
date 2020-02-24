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

#include "macros.hpp"
#include "math/math.hpp"
#include "types.hpp"

#include <Scene/BsSceneManager.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// Factory
// ========================================================================== //

namespace wind {

class EditorFactory {
  WIND_NAMESPACE_CLASS(EditorFactory);

public:
  /// Creates an empty scene with the specified name
  ///
  /// The scene contains the following objects:
  /// - "root": Root of the scene
  ///   - "geometry": Root of all geometry
  ///     - "geometry_plane": Root of all geometry
  static bs::HSceneObject createEmptyScene(const bs::String &name,
                                           f32 groundScale);

  /// Creates a default scene (Note CLEAR THE SCENE AFTER CALLING THIS). The
  /// name of the root object is specified as well as the scale of the
  /// ground-plane.
  ///
  /// The created scene has a specific structure. An overview of the
  /// structure with the name for each object is presented here
  ///
  /// Overview:
  /// - "root": Root of the scene
  ///   - "geometry": Root of all geometry
  ///     - "geometry_plane": Root of all geometry
  ///     - "geometry_box": Root of all geometry
  ///     - "geometry_bunny": Root of all geometry
  static bs::HSceneObject createDefaultScene(const bs::String &name,
                                             f32 groundScale);

public:
  /// Name of the camera object
  static constexpr char OBJ_CAMERA_NAME[] = "camera";
};

} // namespace wind