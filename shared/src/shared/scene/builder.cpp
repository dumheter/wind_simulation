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

#include "shared/scene/builder.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/asset.hpp"
#include "shared/log.hpp"
#include "shared/utility/json_util.hpp"
#include "shared/utility/util.hpp"

#include <thirdparty/alflib/core/assert.hpp>
#include <thirdparty/alflib/file/path.hpp>

#include <Components/BsCBoxCollider.h>
#include <Components/BsCPlaneCollider.h>
#include <Components/BsCRenderable.h>
#include <Components/BsCRigidbody.h>
#include <Components/BsCSkybox.h>
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
  case Kind::kModel: {
    break;
  }
  case Kind::kEmpty:
  default: {
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

ObjectBuilder &ObjectBuilder::withScale(const Vec3F &scale) {
  m_handle->setScale(scale);

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withMaterial(const String &texPath,
                                           const Vec2F &tiling) {
  const bs::HShader shader =
      bs::gBuiltinResources().getBuiltinShader(bs::BuiltinShader::Standard);
  bs::HTexture texture = Asset::loadTexture(texPath);
  m_material = bs::Material::create(shader);
  m_material->setTexture("gAlbedoTex", texture);
  m_material->setVec2("gUVTile", tiling);

  if (m_renderable != nullptr) {
    m_renderable->setMaterial(m_material);
  }
  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withSkybox(const String &path) {
  AlfAssert(m_kind == Kind::kSkybox,
            "Only skybox objects can have a skybox component added");

  const bs::HTexture tex = Asset::loadCubemap(path);
  bs::HSkybox comp = m_handle->addComponent<bs::CSkybox>();
  comp->setTexture(tex);

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withPhysics(f32 restitution, f32 mass) {
  bs::HPhysicsMaterial material =
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
  default: {
    break;
  }
  }

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withRigidbody() {
  bs::HRigidbody rigidbody = m_handle->addComponent<bs::CRigidbody>();

  return *this;
}

// -------------------------------------------------------------------------- //

ObjectBuilder &ObjectBuilder::withObject(const bs::HSceneObject &object) {
  object->setParent(m_handle);

  return *this;
}

// -------------------------------------------------------------------------- //

bs::HSceneObject ObjectBuilder::build() {
  // Check and possibly auto-generate name
  if (m_handle->getName().empty()) {
    m_handle->setName(allocDefaultName());
  }

  // Reset handle and return built scene
  bs::HSceneObject tmpHandle = m_handle;
  m_handle = bs::SceneObject::create("");
  return tmpHandle;
}

// -------------------------------------------------------------------------- //

bs::HSceneObject ObjectBuilder::fromJson(const String &directory,
                                         const nlohmann::json &value) {
  // Find type
  std::string type = value.value("type", "empty");
  ObjectBuilder::Kind kind = kindFromString(type.c_str());
  ObjectBuilder builder(kind);

  // Kind-specific options
  switch (kind) {
  case ObjectBuilder::Kind::kSkybox: {
    auto cubemapIt = value.find("texture");
    if (cubemapIt != value.end()) {
      builder.withSkybox(directory + "\\" + String(*cubemapIt));
    } else {
      logError("Cubemap objects require a \"texture\" property");
      return bs::SceneObject::create("");
    }
    break;
  }
  case Kind::kPlane: {
    Vec2F tiling = JsonUtil::getVec2F(value, "tiling", Vec2F::ONE);
    String tex = value.value("texture", "").c_str();
    if (!tex.empty()) {
      builder.withMaterial(directory + "\\" + tex, tiling);
    }
    break;
  }
  case Kind::kCube: {
    Vec2F tiling = JsonUtil::getVec2F(value, "tiling", Vec2F::ONE);
    String tex = value.value("texture", "").c_str();
    if (!tex.empty()) {
      builder.withMaterial(directory + "\\" + tex, tiling);
    }
    break;
  }
  default: {
    break;
  }
  }

  // Common options
  String name = JsonUtil::getOrCall<String>(value, String("name"), []() {
    return ObjectBuilder::allocDefaultName();
  });
  builder.withName(name);

  builder.withPosition(JsonUtil::getVec3F(value, "position"));
  builder.withScale(JsonUtil::getVec3F(value, "scale", Vec3F::ONE));

  if (value.find("physics") != value.end()) {
    f32 restitution = value["physics"].value("restitution", 1.0f);
    f32 mass = value["physics"].value("mass", 0.0f);
    builder.withPhysics(restitution, mass);
  }

  // Sub-objects
  auto it = value.find("objects");
  if (it != value.end()) {
    auto arr = *it;
    if (!arr.is_array()) {
      logError("\"objects\" is required to be an array of scene objects");
      return bs::SceneObject::create("");
    }

    for (auto it = arr.begin(); it != arr.end(); ++it) {
      bs::HSceneObject subObject = fromJson(directory, *it);
      builder.withObject(subObject);
    }
  }

  return builder.build();
}

// -------------------------------------------------------------------------- //

String ObjectBuilder::allocDefaultName() {
  return "object" + String(std::to_string(s_createdCount++));
}

// -------------------------------------------------------------------------- //

wind::ObjectBuilder::Kind
ObjectBuilder::kindFromString(const String &kindString) {
  if (kindString == "empty") {
    return ObjectBuilder::Kind::kEmpty;
  } else if (kindString == "skybox") {
    return ObjectBuilder::Kind::kSkybox;
  } else if (kindString == "plane") {
    return ObjectBuilder::Kind::kPlane;
  } else if (kindString == "cube") {
    return ObjectBuilder::Kind::kCube;
  }
  return ObjectBuilder::Kind::kInvalid;
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

bs::HSceneObject SceneBuilder::fromFile(const String &path) {
  using json = nlohmann::json;

  // Get base directory
  alflib::Path alfPath(path.c_str());
  alflib::String _directory = alfPath.GetDirectory().GetPathString();
  String directory = _directory.GetUTF8();

  // Read file
  String fileContent = Util::readFile(path);
  fileContent = Util::replace(fileContent, '\t', ' ');

  // Parse file
  try {
    json j = json::parse(fileContent.c_str());
    SceneBuilder builder;

    // Name
    if (j.find("name") != j.end()) {
      builder.withName(j["name"]);
    } else {
      builder.withName(allocDefaultName());
    }

    // Objects
    auto objsIt = j.find("objects");
    if (objsIt != j.end()) {
      auto objsArr = *objsIt;
      if (!objsArr.is_array()) {
        logError("\"objects\" is required to be an array of scene objects");
        return bs::SceneObject::create("");
      }

      for (auto it = objsArr.begin(); it != objsArr.end(); ++it) {
        bs::HSceneObject object = ObjectBuilder::fromJson(directory, *it);
        builder.withObject(object);
      }
    }

    // Done
    return builder.build();
  } catch (std::exception &e) {
    logError("Exception while parsing json file \"{}\": {}", path.c_str(),
             e.what());
  }

  // Failed to parse, return empty scene
  return bs::SceneObject::create("");
}

// -------------------------------------------------------------------------- //

String SceneBuilder::allocDefaultName() {
  return "scene" + String(std::to_string(s_createdCount++));
}

// -------------------------------------------------------------------------- //

u32 SceneBuilder::s_createdCount = 0;

} // namespace wind