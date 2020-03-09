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

#include "shared/macros.hpp"
#include "shared/types.hpp"

#include <BsPrerequisites.h>
#include <ThirdParty/json.hpp>

// ========================================================================== //
// Scene Declaration
// ========================================================================== //

namespace wind {

///
class Scene {
  WIND_NAMESPACE_CLASS(Scene);

public:
  /// Load a scene from a JSON file at the specified path.
  static bs::HSceneObject load(const String &path);

  /// Save a scene to a JSON file at the specified path.
  static void save(const String &path, const bs::HSceneObject &scene);

  /// Private function to load a scene from a JSON value. All resources
  /// are assumed to be relative to the working directory.
  static bs::HSceneObject loadScene(const nlohmann::json &value);

  /// Private function to load a scene object from a JSON value. All resources
  /// are assumed to be relative to the working directory.
  static bs::HSceneObject loadObject(const nlohmann::json &value);

  ///
  static nlohmann::json saveScene(const bs::HSceneObject &scene);

  ///
  static nlohmann::json saveObject(const bs::HSceneObject &object);
};

} // namespace wind
