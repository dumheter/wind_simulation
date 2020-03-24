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

#include "factory.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/asset.hpp"
#include "shared/scene/builder.hpp"

#include <Material/BsMaterial.h>

// ========================================================================== //
// EditorFactory Implementation
// ========================================================================== //

namespace wind {

bs::HSceneObject EditorFactory::createEmptyScene(const bs::String &name,
                                                 f32 groundScale) {

  using namespace bs;

  HSceneObject plane =
      ObjectBuilder(ObjectBuilder::Kind::kPlane)
          .withName("plane")
          .withScale(Vec3F(groundScale, 1.0f, groundScale))
          .withPosition(Vec3F(groundScale, .4f, groundScale))
          .withMaterial(ObjectBuilder::ShaderKind::kStandard,
                        "../res/textures/grid.png",
                        Vec2F(groundScale * 4.0f, groundScale * 4.0f))
          .withCollider(0.5f)
          .build();

  HSceneObject geometry = ObjectBuilder(ObjectBuilder::Kind::kEmpty)
                              .withName("geometry")
                              .withObject(plane)
                              .build();

  HSceneObject skybox = ObjectBuilder(ObjectBuilder::Kind::kEmpty)
                            .withSkybox("../res/skybox/daytime.hdr")
                            .build();

  HSceneObject scene = SceneBuilder{}
                           .withName(name)
                           .withObject(geometry)
                           .withObject(skybox)
                           .build();

  return scene;
}

// -------------------------------------------------------------------------- //

bs::HSceneObject EditorFactory::createDefaultScene(const bs::String &name,
                                                   f32 groundScale) {
  using namespace bs;

  HSceneObject plane =
      ObjectBuilder(ObjectBuilder::Kind::kPlane)
          .withName("plane")
          .withScale(Vec3F(groundScale, 1.0f, groundScale))
          .withPosition(Vec3F(groundScale, .4f, groundScale))
          .withMaterial(ObjectBuilder::ShaderKind::kStandard,
                        "../res/textures/grid.png",
                        Vec2F(groundScale * 4.0f, groundScale * 4.0f))
          .withCollider(0.5f)
          .build();

  HSceneObject cube = ObjectBuilder(ObjectBuilder::Kind::kCube)
                          .withName("cube")
                          .withScale(Vec3F(1.0f, 6.5f, 1.5f))
                          .withPosition(Vec3F(6.0f, 3.0f, 4.0f))
                          .withMaterial(ObjectBuilder::ShaderKind::kStandard,
                                        "../res/textures/grid_2.png")
                          .withCollider(0.5f, 25.0f)
                          .withRigidbody()
                          .build();

  HSceneObject geometry = ObjectBuilder(ObjectBuilder::Kind::kEmpty)
                              .withName("geometry")
                              .withObject(plane)
                              .withObject(cube)
                              .build();

  HSceneObject skybox = ObjectBuilder(ObjectBuilder::Kind::kEmpty)
                            .withSkybox("../res/skybox/daytime.hdr")
                            .build();

  HSceneObject scene = SceneBuilder{}
                           .withName(name)
                           .withObject(skybox)
                           .withObject(geometry)
                           .build();

  return scene;
}

} // namespace wind
