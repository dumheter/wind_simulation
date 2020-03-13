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

#include "shared/scene/builder.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/asset.hpp"
#include "shared/log.hpp"
#include "shared/render/shader.hpp"
#include "shared/scene/cnet_component.hpp"
#include "shared/scene/component/cspline.hpp"
#include "shared/scene/component/ctag.hpp"
#include "shared/scene/component/cwind.hpp"
#include "shared/scene/crotor.hpp"
#include "shared/scene/fps_walker.hpp"
#include "shared/state/moveable_state.hpp"
#include "shared/utility/util.hpp"

#include <Components/BsCBoxCollider.h>
#include <Components/BsCCharacterController.h>
#include <Components/BsCMeshCollider.h>
#include <Components/BsCPlaneCollider.h>
#include <Components/BsCRenderable.h>
#include <Components/BsCRigidbody.h>
#include <Components/BsCSkybox.h>
#include <Components/BsCSphereCollider.h>
#include <Material/BsMaterial.h>
#include <Physics/BsPhysicsMaterial.h>
#include <Resources/BsBuiltinResources.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// ObjectBuilder Implementation
// ========================================================================== //

namespace wind {

ObjectBuilder::ObjectBuilder(Kind kind)
    : m_handle(bs::SceneObject::create("")), m_kind(kind) {
  m_handle->addComponent<CTag>(kind);

  // Create by kind
  switch (kind) {
  case Kind::kEmpty: {
    break;
  }
  case Kind::kPlane: {
    const bs::HMesh mesh =
        bs::gBuiltinResources().getMesh(bs::BuiltinMesh::Quad);
    withMesh(mesh);
    break;
  }
  case Kind::kCube: {
    const bs::HMesh mesh =
        bs::gBuiltinResources().getMesh(bs::BuiltinMesh::Box);
    withMesh(mesh);
    break;
  }
  case Kind::kBall: {
    const bs::HMesh mesh =
        bs::gBuiltinResources().getMesh(bs::BuiltinMesh::Sphere);
    withMesh(mesh);
    break;
  }
  case Kind::kModel: {
    break;
  }
  case Kind::kPlayer: {
    bs::HCharacterController charController =
        m_handle->addComponent<bs::CCharacterController>();
    charController->setHeight(1.0f);
    charController->setRadius(0.4f);
    auto fpsWalker = m_handle->addComponent<FPSWalker>();

    auto prep = ObjectBuilder{ObjectType::kCylinder}
                    .withSave(false)
                    .withName("playerRep")
                    .withPosition(bs::Vector3(0.0f, -0.1f, 0.0f))
                    .withScale(bs::Vector3(0.7f, 1.8f, 0.7f))
                    .withMaterial(ObjectBuilder::ShaderKind::kStandard,
                                  "res/textures/grid_bg.png")
                    .build();

    withObject(prep);
    break;
  }
  case Kind::kRotor: {
    bs::HMesh mesh = Asset::loadMesh("res/meshes/rotor.fbx", 0.1f);
    withMesh(mesh);
    break;
  }
  case Kind::kCylinder: {
    auto [mesh, _] = Asset::loadMeshWithPhysics("res/meshes/cylinder.fbx", 1.0f,
                                                false, true);
    withMesh(mesh);
    break;
  }
  default: {
    Util::panic("Invalid type when building object ({})", kind);
  }
  }
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withName(const String &name) {
  m_handle->setName(name);

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withSave(bool save) {
  const HCTag ctag = m_handle->getComponent<CTag>();
  ctag->getData().save = save;

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withPosition(const Vec3F &position) {
  m_handle->setPosition(position);

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withRotation(const Vec3F &rotation) {
  m_handle->setRotation(bs::Quaternion(
      bs::Degree(rotation.x), bs::Degree(rotation.y), bs::Degree(rotation.z)));
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withRotation(const Quat &rotation) {
  m_handle->setRotation(rotation);
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withScale(const Vec3F &scale) {
  m_handle->setScale(scale);

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withMesh(const bs::HMesh &mesh) {
  m_renderable = m_handle->addComponent<bs::CRenderable>();
  m_renderable->setMesh(mesh);

  if (m_material != nullptr) {
    m_renderable->setMaterial(m_material);
  }

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withMaterial(ShaderKind shaderKind,
                                           const String &texPath,
                                           const Vec2F &tiling,
                                           const Vec4F &color) {
  // Determine shader
  bs::HShader shader;
  switch (shaderKind) {
  case ShaderKind::kTransparent: {
    shader = ShaderManager::getTransparent();
    break;
  }
  case ShaderKind::kTransparentNoCull: {
    shader = ShaderManager::getTransparentNoCull();
    break;
  }
  case ShaderKind::kWireframe: {
    shader = ShaderManager::getWireframe();
    break;
  }
  case ShaderKind::kStandard:
  default: {
    shader = ShaderManager::getDiffuse();
    break;
  }
  }

  // Setup material
  m_material = bs::Material::create(shader);
  if (shaderKind != ShaderKind::kWireframe) {
    const bs::HTexture texture = AssetManager::loadTexture(texPath);
    m_material->setTexture("gAlbedoTex", texture);
    m_material->setVec2("gUVTile", tiling);
  }
  m_material->setVec4("gTintColor", color);

  const HCTag ctag = m_handle->getComponent<CTag>();
  ctag->getData().mat.albedo = texPath;
  ctag->getData().mat.tiling = tiling;
  ctag->getData().mat.shader = stringFromShaderKind(shaderKind);
  ctag->getData().mat.color = color;

  // Set
  if (m_renderable != nullptr) {
    m_renderable->setMaterial(m_material);
  }
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withSkybox(const String &path) {
  const bs::HTexture tex = Asset::loadCubemap(path);
  bs::HSkybox comp = m_handle->addComponent<bs::CSkybox>();
  comp->setTexture(tex);

  HCTag ctag = m_handle->getComponent<CTag>();
  ctag->getData().skybox = path;

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withCollider(f32 restitution, f32 mass, u32 layer,
                                           bool trigger, bool report) {
  return withCollider(m_kind, restitution, mass, layer, trigger, report);
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withCollider(Kind kind, f32 restitution, f32 mass,
                                           u32 layer, bool trigger,
                                           bool report) {
  // Store tag information
  const HCTag ctag = m_handle->getComponent<CTag>();
  ctag->getData().physics.collider = true;
  ctag->getData().physics.restitution = restitution;
  ctag->getData().physics.mass = mass;

  // Setup physics material
  const bs::HPhysicsMaterial material =
      bs::PhysicsMaterial::create(1.0f, 1.0f, restitution);

  // Setup collider
  switch (kind) {
  case Kind::kPlane: {
    bs::HPlaneCollider collider = m_handle->addComponent<bs::CPlaneCollider>();
    collider->setMaterial(material);
    collider->setMass(mass);
    collider->setIsTrigger(trigger);
    collider->setLayer(layer);
    if (report) {
      collider->setCollisionReportMode(bs::CollisionReportMode::Report);
    }
    break;
  }
  case Kind::kCube: {
    bs::HBoxCollider collider = m_handle->addComponent<bs::CBoxCollider>();
    collider->setMaterial(material);
    collider->setMass(mass);
    collider->setIsTrigger(trigger);
    collider->setLayer(layer);
    if (report) {
      collider->setCollisionReportMode(bs::CollisionReportMode::Report);
    }
    break;
  }
  case Kind::kBall: {
    bs::HSphereCollider collider =
        m_handle->addComponent<bs::CSphereCollider>();
    collider->setMaterial(material);
    collider->setMass(mass);
    collider->setIsTrigger(trigger);
    collider->setLayer(layer);
    if (report) {
      collider->setCollisionReportMode(bs::CollisionReportMode::Report);
    }
    break;
  }
  case Kind::kRotor: {
    bs::HBoxCollider collider = m_handle->addComponent<bs::CBoxCollider>();
    collider->setMaterial(material);
    collider->setMass(mass);
    collider->setExtents(Vec3F(1.8f, 0.1f, 1.8f));
    collider->setIsTrigger(trigger);
    collider->setLayer(layer);
    if (report) {
      collider->setCollisionReportMode(bs::CollisionReportMode::Report);
    }
    break;
  }
  case Kind::kCylinder: {
    bs::HMeshCollider collider = m_handle->addComponent<bs::CMeshCollider>();
    auto [_, pmesh] = Asset::loadMeshWithPhysics("res/meshes/cylinder.fbx",
                                                 1.0f, false, true);
    collider->setMesh(pmesh);
    collider->setMaterial(material);
    collider->setMass(mass);
    collider->setIsTrigger(trigger);
    collider->setLayer(layer);
    if (report) {
      collider->setCollisionReportMode(bs::CollisionReportMode::Report);
    }
    break;
  }
  default: {
    break;
  }
  }

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withTriggerCollider(Kind kind, u32 layer) {
  return withCollider(kind, 1.0f, 0.0f, layer, true, true);
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withRigidbody() {
  const HCTag ctag = m_handle->getComponent<CTag>();
  ctag->getData().physics.rigidbody = true;
  bs::HRigidbody rigidbody = m_handle->addComponent<bs::CRigidbody>();
  rigidbody->setSleepThreshold(0.1f);

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &
ObjectBuilder::withWindSource(const std::vector<BaseFn> &functions,
                              WindSystem::VolumeType volumeType) {
  auto wind = m_handle->addComponent<CWind>(volumeType);
  wind->addFunctions(functions);
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withSpline(const std::vector<Vec3F> &points,
                                         u32 degree, u32 samples) {
  // Spline component
  HCSpline splineComp = m_handle->addComponent<CSpline>(points, degree);

  // Add sub-objects to render spline
  Spline *spline = splineComp->getSpline();
  const f32 len = spline->calcLen(u32(spline->getPoints().size()) * 10u);
  if (samples == kSplineSamplesInvalid) {
    samples = u32(len);
  }

  ObjectBuilder splineObjBuilder(ObjectType::kEmpty);
  splineObjBuilder.withName("spline_waypoints");

  const f32 step = 1.0f / samples;
  for (u32 i = 0; i <= samples; i++) {
    f32 t = step * i;
    Vec3F pos = spline->sample(t);

    const bs::HSceneObject waypointObj =
        ObjectBuilder{ObjectType::kCube}
            .withSave(false)
            .withMaterial(ShaderKind::kStandard, "res/textures/white.png",
                          Vec2F::ONE, Vec4F(1.0f, 0.0f, 0.0f, 1.0f))
            .withPosition(pos)
            .withScale(Vec3F(0.02f, 0.02f, 0.02f))
            .build();

    splineObjBuilder.withObject(waypointObj);
  }
  const bs::HSceneObject splineObj = splineObjBuilder.build();
  withObject(splineObj);

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &
ObjectBuilder::withSplineFollow(f32 speed, CSplineFollow::WrapMode wrapMode) {
  HCSplineFollow comp = m_handle->addComponent<CSplineFollow>(speed, wrapMode);

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withRotor(Vec3F rotation) {
  auto rotor = m_handle->addComponent<CRotor>(rotation);
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withObject(const bs::HSceneObject &object) {
  object->setParent(m_handle);

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &
ObjectBuilder::withNetComponent(const MoveableState &moveableState) {
  auto netComp = m_handle->addComponent<CNetComponent>(moveableState);
  const HCTag ctag = m_handle->getComponent<CTag>();
  ctag->getData().id = std::make_optional(moveableState.id);
  return *this;
}

ObjectBuilder &ObjectBuilder::withNetComponent(UniqueId id) {
  MoveableState state{};
  state.id = id;
  m_handle->addComponent<CNetComponent>(state);
  HCTag ctag = m_handle->getComponent<CTag>();
  ctag->getData().id = std::make_optional(id);
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withWindVolume(WindSystem::VolumeType type,
                                             Vec4F color) {
  Vec3F pos{0.0f, 0.0f, 0.0f};
  Vec3F rot{1.0f, 1.0f, 1.0f};

  switch (type) {
  case WindSystem::VolumeType::kCube: {
    Vec3F scale{1.0f, 1.0f, 1.0f};
    withDebugCube(scale, pos, rot, color);
    break;
  }
  case WindSystem::VolumeType::kCylinder: {
    f32 radius = 1.0f;
    f32 height = 1.0f;
    withDebugCylinder(radius, height, pos, rot, color);
    break;
  }
  }
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &
ObjectBuilder::withWindOccluder(const CWindOccluder::Cube &cube) {
  HCWindOccluder comp = m_handle->addComponent<CWindOccluder>(cube);
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &
ObjectBuilder::withWindOccluder(const CWindOccluder::Cylinder &cylinder) {
  HCWindOccluder comp = m_handle->addComponent<CWindOccluder>(cylinder);
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withDebugCube(const Vec3F &size,
                                            const Vec3F &position,
                                            const Vec3F &rotation,
                                            const Vec4F &color) {
  const bs::HSceneObject obj =
      ObjectBuilder(ObjectType::kCube)
          .withSave(false)
          .withMaterial(ShaderKind::kTransparentNoCull,
                        "res/textures/white.png", Vec2F::ONE, color)
          .withPosition(position)
          .withScale(size)
          .withRotation(rotation)
          .build();
  withObject(obj);
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withDebugCylinder(f32 radius, f32 height,
                                                const Vec3F &position,
                                                const Vec3F &rotation,
                                                const Vec4F &color) {
  const bs::HSceneObject obj =
      ObjectBuilder(ObjectType::kCylinder)
          .withSave(false)
          .withMaterial(ShaderKind::kTransparentNoCull,
                        "res/textures/white.png", Vec2F::ONE, color)
          .withPosition(position)
          .withScale(Vec3F(radius, height, radius))
          .withRotation(rotation)
          .build();
  withObject(obj);
  return *this;
}

// -------------------------------------------------------------------------- //

bs::HSceneObject ObjectBuilder::build() {
  // Check and possibly auto-generate name
  if (m_handle->getName().empty()) {
    m_handle->setName(nextName());
  }

  // Reset handle and return built scene
  bs::HSceneObject tmpHandle = m_handle;
  m_handle = bs::SceneObject::create("");
  return tmpHandle;
}

// -------------------------------------------------------------------------- //

String ObjectBuilder::nextName() {
  return "object" + String(std::to_string(s_createdCount++));
}

// -------------------------------------------------------------------------- //

ObjectBuilder::Kind ObjectBuilder::kindFromString(const String &kindString) {
  if (kindString == "empty") {
    return Kind::kEmpty;
  } else if (kindString == "plane") {
    return Kind::kPlane;
  } else if (kindString == "cube") {
    return Kind::kCube;
  } else if (kindString == "ball") {
    return Kind::kBall;
  } else if (kindString == "model") {
    return Kind::kModel;
  } else if (kindString == "player") {
    return Kind::kPlayer;
  } else if (kindString == "rotor") {
    return Kind::kRotor;
  } else if (kindString == "cylinder") {
    return Kind::kCylinder;
  }
  return Kind::kInvalid;
}

// -------------------------------------------------------------------------- //

String ObjectBuilder::stringFromKind(Kind kind) {
  switch (kind) {
  case Kind::kEmpty:
    return "empty";
  case Kind::kPlane:
    return "plane";
  case Kind::kCube:
    return "cube";
  case Kind::kBall:
    return "sphere";
  case Kind::kModel:
    return "model";
  case Kind::kPlayer:
    return "player";
  case Kind::kRotor:
    return "rotor";
  case Kind::kCylinder:
    return "cylinder";
  case Kind::kInvalid:
  default: {
    return "invalid";
  }
  }
}

// -------------------------------------------------------------------------- //

ObjectBuilder::ShaderKind
ObjectBuilder::shaderKindFromString(const String &kind) {
  if (kind == "transparent") {
    return ShaderKind::kTransparent;
  }
  if (kind == "transparent_no_cull") {
    return ShaderKind::kTransparentNoCull;
  }
  if (kind == "wireframe") {
    return ShaderKind::kWireframe;
  }
  return ShaderKind::kStandard;
}

// -------------------------------------------------------------------------- //

String ObjectBuilder::stringFromShaderKind(ShaderKind kind) {
  switch (kind) {
  case ShaderKind::kStandard:
    return "standard";
  case ShaderKind::kTransparent:
    return "transparent";
  case ShaderKind::kTransparentNoCull:
    return "transparent_no_cull";
  case ShaderKind::kWireframe:
    return "wireframe";
  default:
    Util::panic("Invalid shader kind");
  }
}

// -------------------------------------------------------------------------- //

u32 ObjectBuilder::s_createdCount = 0;

} // namespace wind

// ========================================================================== //
// SceneBuilder Implementation
// ========================================================================== //

namespace wind {

SceneBuilder::SceneBuilder() { m_handle = bs::SceneObject::create(""); }

// -------------------------------------------------------------------------- //

SceneBuilder &SceneBuilder::withName(const String &name) {
  m_handle->setName(name);

  return *this;
}

// -------------------------------------------------------------------------- //

SceneBuilder &SceneBuilder::withObject(const bs::HSceneObject &object) {
  object->setParent(m_handle);

  return *this;
}

// -------------------------------------------------------------------------- //

bs::HSceneObject SceneBuilder::build() {
  // Check and possibly auto-generate name
  if (m_handle->getName().empty()) {
    String name = "scene" + String(std::to_string(s_createdCount++));
    m_handle->setName(name);
  }

  // Reset handle and return built scene
  bs::HSceneObject tmpHandle = m_handle;
  m_handle = bs::SceneObject::create("");
  return tmpHandle;
}

// -------------------------------------------------------------------------- //

String SceneBuilder::nextName() {
  return "scene" + String(std::to_string(s_createdCount++));
}

// -------------------------------------------------------------------------- //

u32 SceneBuilder::s_createdCount = 0;

} // namespace wind
