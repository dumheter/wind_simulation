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

#include "shared/scene/scene.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/asset.hpp"
#include "shared/log.hpp"
#include "shared/scene/builder.hpp"
#include "shared/utility/json_util.hpp"
#include "shared/utility/util.hpp"

#include <thirdparty/alflib/core/assert.hpp>
#include <thirdparty/alflib/file/path.hpp>

#include <Components/BsCRenderable.h>
#include <Components/BsCSkybox.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// Scene Implementation
// ========================================================================== //

namespace wind {

bs::HSceneObject Scene::load(const String &path) {
  using json = nlohmann::json;

  // Read file
  String fileContent = Util::readFile(path);
  fileContent = Util::replace(fileContent, '\t', ' ');

  // Parse file
  json value;
  try {
    value = json::parse(fileContent.c_str());
  } catch (std::exception &e) {
    logError("Exception while parsing json file \"{}\": {}", path.c_str(),
             e.what());
    return bs::SceneObject::create("");
  }

  return loadScene(value);
}

// -------------------------------------------------------------------------- //

void Scene::save(const String &path, const bs::HSceneObject &scene) {
  using json = nlohmann::json;
  json value = saveScene(scene);
  Util::writeFile(path, value.dump(2).c_str());
}

// -------------------------------------------------------------------------- //

bs::HSceneObject Scene::loadScene(const nlohmann::json &value) {
  SceneBuilder builder;

  // Name
  if (value.find("name") != value.end()) {
    builder.withName(value["name"]);
  } else {
    builder.withName(SceneBuilder::nextName());
  }

  // Objects
  auto objsIt = value.find("objects");
  if (objsIt != value.end()) {
    auto objsArr = *objsIt;
    if (!objsArr.is_array()) {
      logError("\"objects\" is required to be an array of scene objects");
      return bs::SceneObject::create("");
    }

    for (auto &it : objsArr) {
      bs::HSceneObject object = loadObject(it);
      builder.withObject(object);
    }
  }

  // Done
  return builder.build();
}

// -------------------------------------------------------------------------- //

bs::HSceneObject Scene::loadObject(const nlohmann::json &value) {
  using json = nlohmann::json;

  // Find type
  std::string type = value.value("type", "empty");
  ObjectBuilder::Kind kind = ObjectBuilder::kindFromString(type.c_str());
  ObjectBuilder builder(kind);

  // Kind-specific options
  switch (kind) {
  case ObjectBuilder::Kind::kPlane: {
    Vec2F tiling = JsonUtil::getVec2F(value, "tiling", Vec2F::ONE);
    String tex = value.value("texture", "").c_str();
    if (!tex.empty()) {
      builder.withMaterial(tex, tiling);
    }
    break;
  }
  case ObjectBuilder::Kind::kCube: {
    Vec2F tiling = JsonUtil::getVec2F(value, "tiling", Vec2F::ONE);
    String tex = value.value("texture", "").c_str();
    if (!tex.empty()) {
      builder.withMaterial(tex, tiling);
    }
    break;
  }
  default: {
    break;
  }
  }

  // Common options
  String name = JsonUtil::getOrCall<String>(
      value, String("name"), []() { return ObjectBuilder::nextName(); });
  builder.withName(name);

  builder.withPosition(JsonUtil::getVec3F(value, "position"));
  builder.withScale(JsonUtil::getVec3F(value, "scale", Vec3F::ONE));

  if (value.find("physics") != value.end()) {
    f32 restitution = value["physics"].value("restitution", 1.0f);
    f32 mass = value["physics"].value("mass", 0.0f);
    builder.withPhysics(restitution, mass);
  }

  // Skybox
  if (value.find("skybox") != value.end()) {
    json skybox = value["skybox"];
    if (skybox.find("cubemap") != skybox.end()) {
      String cubemapPath = String(skybox["cubemap"]);
      builder.withSkybox(cubemapPath);
    } else {
      logError("Skybox requires a \"cubemap\" property");
      return bs::SceneObject::create("");
    }
  }

  // Sub-objects
  auto it = value.find("objects");
  if (it != value.end()) {
    auto arr = *it;
    if (!arr.is_array()) {
      logError("\"objects\" is required to be an array of scene objects");
      return bs::SceneObject::create("");
    }

    for (auto &it : arr) {
      bs::HSceneObject subObject = loadObject(it);
      builder.withObject(subObject);
    }
  }

  return builder.build();
}

// -------------------------------------------------------------------------- //

nlohmann::json Scene::saveScene(const bs::HSceneObject &scene) {
  using json = nlohmann::json;

  // Scene
  json value;
  value["name"] = scene->getName().c_str();

  // Objects
  json objArrValue;
  u32 cnt = scene->getNumChildren();
  for (u32 i = 0; i < cnt; i++) {
    bs::HSceneObject object = scene->getChild(i);
    objArrValue.push_back(saveObject(object));
  }
  value["objects"] = objArrValue;

  return value;
}

// -------------------------------------------------------------------------- //

nlohmann::json Scene::saveObject(const bs::HSceneObject &object) {
  using json = nlohmann::json;

  // Object
  json value;
  value["name"] = object->getName();

  HCTag tag = object->getComponent<CTag>();
  if (tag->getType() != ObjectType::kEmpty) {
    value["type"] = ObjectBuilder::stringFromKind(tag->getType());
  }

  // Skybox
  if (object->getComponent<bs::CSkybox>()) {
    value["skybox"]["cubemap"] = tag->getPathSkybox();
  }

  // Material
  if (object->getComponent<bs::CRenderable>()) {
    JsonUtil::setValue(value, "tiling", tag->getTexTiling());
    value["texture"] = tag->getPathAlbedo();
  }

  // Position
  Vec3F position = object->getTransform().getPosition();
  if (position != Vec3F::ZERO) {
    JsonUtil::setValue(value, "position", position);
  }

  Vec3F scale = object->getTransform().getScale();
  if (scale != Vec3F::ONE) {
    JsonUtil::setValue(value, "scale", scale);
  }

  Quat rotation = object->getTransform().getRotation();
  if (rotation != Quat::IDENTITY) {
    JsonUtil::setValue(value, "rotation", rotation);
  }

  // Sub-objects
  u32 cnt = object->getNumChildren();
  if (cnt > 0) {
    json objArrValue;
    for (u32 i = 0; i < cnt; i++) {
      bs::HSceneObject subObject = object->getChild(i);
      objArrValue.push_back(saveObject(subObject));
    }
    value["objects"] = objArrValue;
  }

  return value;
}

} // namespace wind