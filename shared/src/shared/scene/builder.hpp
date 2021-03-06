// MIT License
//
// Copyright (c) 2020 Filip Bj�rklund, Christoffer Gustafsson
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

#include "shared/math/math.hpp"
#include "shared/scene/component/cspline_follow.hpp"
#include "shared/scene/component/cwind_occluder.hpp"
#include "shared/scene/types.hpp"
#include "shared/types.hpp"
#include "shared/utility/unique_id.hpp"
#include "shared/wind/wind_system.hpp"

#include <vector>

#include "component/cpaint.hpp"

// ========================================================================== //
// ObjectBuilder
// ========================================================================== //

namespace wind {

struct BaseFn;
struct MoveableState;

class ObjectBuilder {
public:
  /// Invalid number of samples. Used as default
  static constexpr u32 kSplineSamplesAuto = std::numeric_limits<u32>::max();

  /// Enumeration of shader kinds. An object material uses one of these.
  enum class ShaderKind {
    kStandard,          ///< Standard PBR shader.
    kTransparent,       ///< Transparent shader.
    kTransparentNoCull, ///< Transparent shader with no back-face culling
    kWireframe          ///< Wireframe shader.
  };

  /// Object kind
  using Kind = ObjectType;

  /// Construct object builder
  ObjectBuilder(Kind kind = Kind::kEmpty);

  /// Set the name
  ObjectBuilder &withName(const String &name);

  /// Set whether object is saved
  ObjectBuilder &withSave(bool save = true);

  /// Set the position
  ObjectBuilder &withPosition(const Vec3F &position);

  /// Sets the rotation
  ObjectBuilder &withRotation(const Vec3F &rotation);

  /// Sets the rotation
  ObjectBuilder &withRotation(const Quat &rotation);

  /// Sets the scale
  ObjectBuilder &withScale(const Vec3F &scale);

  /// Set the mesh
  ObjectBuilder &withMesh(const bs::HMesh &mesh);

  /// Set the material of the object to one that uses the specified shader and
  /// has the specified texture. Tiling for the texture and a tint color can
  /// also be specified. Whether color is applicable depends on the shader.
  ObjectBuilder &withMaterial(ShaderKind shaderKind, const String &texPath,
                              const Vec2F &tiling = Vec2F::ONE,
                              const Vec4F &color = Vec4F(1.0f, 1.0f, 1.0f,
                                                         1.0f));

  /// Sets skybox texture
  ObjectBuilder &withSkybox(const String &path);

  /// Adds a collider according to object kind
  ObjectBuilder &withCollider(f32 restitution = 1.0f, f32 mass = 0.0f,
                              u32 layer = 0, bool trigger = false,
                              bool report = false);

  /// Adds a collider
  ObjectBuilder &withCollider(Kind kind, f32 restitution = 1.0f,
                              f32 mass = 0.0f, u64 layer = 0,
                              bool trigger = false, bool report = false);

  /// Add a trigger collider
  ObjectBuilder &withTriggerCollider(Kind kind, u64 layer = 0,
                                     bool report = false);

  /// Adds a rigidbody
  ObjectBuilder &withRigidbody();

  /// Adds wind
  /// @note Expects its parent to be a cube, and will use its
  /// mesh for the wind collision trigger volume.
  ObjectBuilder &withWindSource(const std::vector<BaseFn> &functions,
                                WindSystem::VolumeType volumeType, Vec3F pos,
                                Vec3F scale);

  ObjectBuilder &withWindVolume(WindSystem::VolumeType type, Vec3F pos,
                                Vec3F scale);

  ObjectBuilder &withWindAffectable();

  /// Add a wind occluder component
  ObjectBuilder &withWindOccluder(const CWindOccluder::Cube &cube);

  /// Add a wind occluder component
  ObjectBuilder &withWindOccluder(const CWindOccluder::Cylinder &cylinder);

  /// Adds a spline component to the object
  ObjectBuilder &withSpline(const std::vector<Vec3F> &points, u32 degree,
                            u32 samples = kSplineSamplesAuto,
                            Vec4F color = Vec4F{1.0f, 0.0f, 0.0f, 1.0f});

  /// Adds a spline follow component to the object
  ObjectBuilder &withSplineFollow(f32 speed, CSplineFollow::WrapMode wrapMode);

  /// Add rotor
  ObjectBuilder &withRotor(Vec3F rotation);

  /// Add sub-object
  ObjectBuilder &withObject(const bs::HSceneObject &object);

  /// @post Must register netcomp in world
  ObjectBuilder &withNetComponent(const MoveableState &moveableState);

  /// @post Must register netcomp in world
  /// Construct from only an id
  ObjectBuilder &withNetComponent(UniqueId id);

  /// Add painter
  ObjectBuilder &withPainter(CPaint::PainterFun &&fun);

  ///
  ObjectBuilder &withDebugCube(const Vec3F &size = Vec3F::ONE,
                               const Vec3F &position = Vec3F::ZERO,
                               const Vec3F &rotation = Vec3F::ZERO,
                               const Vec4F &color = Vec4F{1.0f, 0.0f, 1.0f,
                                                          0.15f});

  ObjectBuilder &
  withDebugCylinder(f32 radius, f32 height, const Vec3F &position = Vec3F::ZERO,
                    const Vec3F &rotation = Vec3F::ZERO,
                    const Vec4F &color = Vec4F{1.0f, 0.0f, 1.0f, 0.15f});

  /// Build the scene object
  bs::HSceneObject build();

  /// Returns the next generic object name
  static String nextName();

  /// Returns an object kind from string
  static Kind kindFromString(const String &kindString);

  /// Returns a string from a kind
  static String stringFromKind(Kind kind);

  static ShaderKind shaderKindFromString(const String &kind);

  static String stringFromShaderKind(ShaderKind kind);

private:
  /// Number of created objects
  static u32 s_createdCount;

  /// Object handle
  bs::HSceneObject m_handle;
  /// Kind
  Kind m_kind;

  /// Material
  bs::HMaterial m_material;
  /// Renderable
  bs::HRenderable m_renderable;
};

} // namespace wind

// ========================================================================== //
// SceneBuilder Declaration
// ========================================================================== //

namespace wind {

///
class SceneBuilder {
public:
  ///
  SceneBuilder();

  SceneBuilder &withName(const String &name);

  SceneBuilder &withObject(const bs::HSceneObject &object);

  bs::HSceneObject build();

  /// Returns the next generic scene name
  static String nextName();

private:
  /// Number of created scenes
  static u32 s_createdCount;

  /// Object handle
  bs::HSceneObject m_handle;
  /// Object list
  std::vector<bs::HSceneObject> m_objects;
};

} // namespace wind
