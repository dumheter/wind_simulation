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
#include "shared/scene/cnet_component.hpp"
#include "shared/scene/component/ctag.hpp"
#include "shared/scene/crotor.hpp"
#include "shared/scene/fps_walker.hpp"
#include "shared/scene/wind_src.hpp"
#include "shared/state/moveable_state.hpp"
#include "shared/utility/json_util.hpp"
#include "shared/utility/util.hpp"

#include <thirdparty/alflib/core/assert.hpp>
#include <thirdparty/alflib/file/path.hpp>

#include <Components/BsCBoxCollider.h>
#include <Components/BsCCharacterController.h>
#include <Components/BsCPlaneCollider.h>
#include <Components/BsCRenderable.h>
#include <Components/BsCRigidbody.h>
#include <Components/BsCSkybox.h>
#include <Components/BsCSphereCollider.h>
#include <Material/BsMaterial.h>
#include <Physics/BsPhysicsMaterial.h>
#include <RenderAPI/BsSamplerState.h>
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
  case Kind::kPlane: {
    bs::HMesh mesh = bs::gBuiltinResources().getMesh(bs::BuiltinMesh::Quad);
    m_renderable = m_handle->addComponent<bs::CRenderable>();
    m_renderable->setMesh(mesh);
    break;
  }
  case Kind::kCube: {
    bs::HMesh mesh = bs::gBuiltinResources().getMesh(bs::BuiltinMesh::Box);
    m_renderable = m_handle->addComponent<bs::CRenderable>();
    m_renderable->setMesh(mesh);
    break;
  }
  case Kind::kBall: {
    bs::HMesh mesh = bs::gBuiltinResources().getMesh(bs::BuiltinMesh::Sphere);
    m_renderable = m_handle->addComponent<bs::CRenderable>();
    m_renderable->setMesh(mesh);
    break;
  }
  case Kind::kModel: {
    break;
  }
  case Kind::kPlayer: {
    logWarning("### built player");
    bs::HCharacterController charController =
        m_handle->addComponent<bs::CCharacterController>();
    charController->setHeight(1.0f);
    charController->setRadius(0.4f);
    auto prep = bs::SceneObject::create("playerRep");
    prep->setParent(m_handle);
    prep->setScale(bs::Vector3(0.3f, 2.0f, 0.3f));
    prep->setPosition(bs::Vector3(0.0f, -1.0f, 0.0f));
    m_renderable = prep->addComponent<bs::CRenderable>();
    bs::HMesh mesh = bs::gBuiltinResources().getMesh(bs::BuiltinMesh::Cylinder);
    m_renderable->setMesh(mesh);
    auto fpsWalker = m_handle->addComponent<FPSWalker>();
    break;
  }
  case Kind::kRotor: {
    bs::HMesh mesh = Asset::loadMesh("res/meshes/rotor.fbx", 0.1f);
    m_renderable = m_handle->addComponent<bs::CRenderable>();
    m_renderable->setMesh(mesh);
    break;
  }
  case Kind::kEmpty: {
    break;
  }
  default: {
    Util::panic("Invalid type when building object ({})", kind);
    break;
  }
  }
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withName(const String &name) {
  m_handle->setName(name);

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

ObjectBuilder &ObjectBuilder::withMaterial(ShaderKind shaderKind,
                                           const String &texPath,
                                           const Vec2F &tiling) {
  // Determine shader
  bs::HShader shader;
  switch (shaderKind) {
  case ShaderKind::kTransparent: {
    shader = bs::gBuiltinResources().getBuiltinShader(
        bs::BuiltinShader::Transparent);
    break;
  }
  case ShaderKind::kStandard:
  default: {
    shader =
        bs::gBuiltinResources().getBuiltinShader(bs::BuiltinShader::Standard);
    break;
  }
  }

  // Setup material
  const bs::HTexture texture = AssetManager::loadTexture(texPath);
  m_material = bs::Material::create(shader);
  m_material->setTexture("gAlbedoTex", texture);
  m_material->setVec2("gUVTile", tiling);

  const HCTag ctag = m_handle->getComponent<CTag>();
  ctag->getData().mat.albedo = texPath;
  ctag->getData().mat.tiling = tiling;
  ctag->getData().mat.shader = stringFromShaderKind(shaderKind);

  // Sampler
  // bs::SAMPLER_STATE_DESC samplerDesc;
  // samplerDesc.minFilter = bs::FilterOptions::FO_LINEAR;
  // samplerDesc.magFilter = bs::FilterOptions::FO_LINEAR;
  // samplerDesc.mipFilter = bs::FilterOptions::FO_LINEAR;
  // const bs::SPtr<bs::SamplerState> sampler =
  //    bs::SamplerState::create(samplerDesc);
  // m_material->setSamplerState("gAlbedoSamp", sampler);

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

ObjectBuilder &ObjectBuilder::withCollider(f32 restitution, f32 mass) {
  const HCTag ctag = m_handle->getComponent<CTag>();
  ctag->getData().physics.collider = true;
  ctag->getData().physics.restitution = restitution;
  ctag->getData().physics.mass = mass;

  const bs::HPhysicsMaterial material =
      bs::PhysicsMaterial::create(1.0f, 1.0f, restitution);

  switch (m_kind) {
  case Kind::kPlane: {
    bs::HPlaneCollider collider = m_handle->addComponent<bs::CPlaneCollider>();
    collider->setMaterial(material);
    collider->setMass(mass);
    break;
  }
  case Kind::kCube: {
    bs::HBoxCollider collider = m_handle->addComponent<bs::CBoxCollider>();
    collider->setMaterial(material);
    collider->setMass(mass);
    break;
  }
  case Kind::kBall: {
    bs::HSphereCollider collider =
        m_handle->addComponent<bs::CSphereCollider>();
    collider->setMaterial(material);
    collider->setMass(mass);
    break;
  }
  case Kind::kRotor: {
    bs::HBoxCollider collider = m_handle->addComponent<bs::CBoxCollider>();
    collider->setMaterial(material);
    collider->setMass(mass);
    collider->setExtents(Vec3F(1.8f, 0.1f, 1.8f));
    break;
  }
  default: {
    break;
  }
  }

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withRigidbody() {
  const HCTag ctag = m_handle->getComponent<CTag>();
  ctag->getData().physics.rigidbody = true;
  bs::HRigidbody rigidbody = m_handle->addComponent<bs::CRigidbody>();
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &
ObjectBuilder::withWindSource(const std::vector<BaseFn> &functions) {
  auto wind = m_handle->addComponent<CWindSource>();
  wind->addFunctions(functions);
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

ObjectBuilder &
ObjectBuilder::withNetComponent(const MoveableState &moveableState) {
  m_handle->addComponent<CNetComponent>(moveableState);
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
  return ShaderKind::kStandard;
}

// -------------------------------------------------------------------------- //

String ObjectBuilder::stringFromShaderKind(ShaderKind kind) {
  switch (kind) {
  case ShaderKind::kStandard:
    return "standard";
  case ShaderKind::kTransparent:
    return "transparent";
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
