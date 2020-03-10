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

#include "shared/scene/scene.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/asset.hpp"
#include "shared/log.hpp"
#include "shared/scene/builder.hpp"
#include "shared/utility/json_util.hpp"
#include "shared/utility/util.hpp"

#include <Components/BsCRenderable.h>
#include <Components/BsCSkybox.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// Scene Implementation
// ========================================================================== //

namespace wind {

bs::HSceneObject Scene::load(const String &scene, const String &source) {
  using json = nlohmann::json;

  json value;
  try {
    value = json::parse(scene.c_str());
  } catch (std::exception &e) {
    String src = "";
    if (!source.empty()) {
      src = fmt::format(" file \"{}\"", source.c_str()).c_str();
    }
    logError("Exception while parsing json{}: {}", src.c_str(), e.what());
    return bs::SceneObject::create("");
  }
  return loadScene(value);
}

// -------------------------------------------------------------------------- //

bs::HSceneObject Scene::loadFile(const String &path) {
  String fileContent = Util::readFile(path);
  fileContent = Util::replace(fileContent, '\t', ' ');
  return load(fileContent.c_str());
}

// -------------------------------------------------------------------------- //

String Scene::save(const bs::HSceneObject &scene) {
  using json = nlohmann::json;
  const json value = saveScene(scene);
  return value.dump(2).c_str();
}

// -------------------------------------------------------------------------- //

void Scene::saveFile(const String &path, const bs::HSceneObject &scene) {
  Util::writeFile(path, save(scene));
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
      logError("[loadScene]\"objects\" is required to be an array of scene "
               "objects: {}",
               value.dump());
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

  // Material
  auto itMat = value.find("material");
  if (itMat != value.end()) {
    json mat = *itMat;
    const Vec2F tiling = JsonUtil::getVec2F(mat, "tiling", Vec2F::ONE);
    const Vec4F color =
        JsonUtil::getVec4F(mat, "color", Vec4F(1.0f, 1.0f, 1.0f, 1.0f));
    const String tex = mat.value("texture", "").c_str();
    const String shaderStr = mat.value("shader", "").c_str();
    const ObjectBuilder::ShaderKind shaderKind =
        ObjectBuilder::shaderKindFromString(shaderStr);

    if (!tex.empty()) {
      builder.withMaterial(shaderKind, tex, tiling);
    } else {
      logWarning("Object has tiling specified but no texture: {}",
                 value.dump());
    }
  }

  // Physics (collider)
  if (value.find("physics") != value.end()) {
    f32 restitution = value["physics"].value("restitution", 1.0f);
    f32 mass = value["physics"].value("mass", 0.0f);
    builder.withCollider(restitution, mass);

    if (value["physics"].value("rigidbody", true)) {
      builder.withRigidbody();
    }
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

  // Wind
  if (value.find("wind") != value.end()) {
    json wind = value["wind"];

    std::vector<BaseFn> functions{};
    for (const auto fn : wind) {
      String windType = JsonUtil::getString(fn, "type", "constant");
      Vec3F dir = JsonUtil::getVec3F(fn, "direction", Vec3F::ZERO);
      f32 mag = fn.value("magnitude", 0.0f);
      functions.push_back(BaseFn::fnConstant(dir, mag));
    }
    builder.withWindSource(functions);
  }

  // Spline
  auto itSpline = value.find("spline");
  if (itSpline != value.end()) {
    json spline = *itSpline;

    u32 degree = JsonUtil::getU32(spline, "degree", 2);
    const auto splinePointsIt = spline.find("points");
    if (splinePointsIt != spline.end()) {
      const nlohmann::json splinePoints = *splinePointsIt;
      if (splinePoints.is_array()) {
        std::vector<Vec3F> points;
        for (const nlohmann::json &splinePoint : splinePoints) {
          points.push_back(JsonUtil::getVec3F(splinePoint));
        }
        builder.withSpline(points, degree);
      }
    } else {
      logError("spline must contain a list of control points");
    }
  }

  // Rotor
  if (value.find("rotor") != value.end()) {
    Vec3F rot = JsonUtil::getVec3F(value, "rotor", Vec3F::ZERO);
    builder.withRotor(rot);
  }

  // Name
  String name = JsonUtil::getOrCall<String>(
      value, String("name"), []() { return ObjectBuilder::nextName(); });
  builder.withName(name);

  // Sub-objects
  auto it = value.find("objects");
  if (it != value.end()) {
    auto arr = *it;
    if (!arr.is_array()) {
      logError("[loadObjects] \"objects\" is required to be an array of scene "
               "objects: {}",
               value.dump());
      return bs::SceneObject::create("");
    }

    for (auto &it0 : arr) {
      bs::HSceneObject subObject = loadObject(it0);
      builder.withObject(subObject);
    }
  }

  // Transform - MUST BE LAST
  builder.withPosition(JsonUtil::getVec3F(value, "position"));
  builder.withScale(JsonUtil::getVec3F(value, "scale", Vec3F::ONE));
  builder.withRotation(JsonUtil::getVec3F(value, "rotation"));

  return builder.build();
}

// -------------------------------------------------------------------------- //

nlohmann::json Scene::saveScene(const bs::HSceneObject &scene) {
  using json = nlohmann::json;

  // Scene
  json value;
  value["name"] = scene->getName().c_str();

  // Objects
  if (u32 cnt = scene->getNumChildren(); cnt > 0) {
    json objArrValue;
    for (u32 i = 0; i < cnt; i++) {
      bs::HSceneObject object = scene->getChild(i);
      objArrValue.push_back(saveObject(object));
    }
    value["objects"] = objArrValue;
  } else {
    value["objects"] = std::vector<json>{};
  }

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
    value["skybox"]["cubemap"] = tag->getData().skybox;
  }

  // Material
  if (object->getComponent<bs::CRenderable>()) {
    if (!tag->getData().mat.albedo.empty()) {
      JsonUtil::setValue(value["material"], "tiling",
                         tag->getData().mat.tiling);
      value["material"]["texture"] = tag->getData().mat.albedo;
    }
    value["material"]["shader"] = tag->getData().mat.shader;
    JsonUtil::setValue(value["material"], "color", tag->getData().mat.color);
  }

  // Physics
  {
    // json mat = value["material"];

    if (!tag->getData().physics.rigidbody) {
      value["physics"]["rigidbody"] = false;
    }
    if (tag->getData().physics.collider) {
      value["physics"]["restitution"] = tag->getData().physics.restitution;
      value["physics"]["mass"] = tag->getData().physics.mass;
    }
  }

  // Position
  const Vec3F position = object->getTransform().getPosition();
  if (position != Vec3F::ZERO) {
    JsonUtil::setValue(value, "position", position);
  }

  const Vec3F scale = object->getTransform().getScale();
  if (scale != Vec3F::ONE) {
    JsonUtil::setValue(value, "scale", scale);
  }

  const Quat rotation = object->getTransform().getRotation();
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
  } else {
    value["objects"] = std::vector<json>{};
  }

  return value;
}

} // namespace wind
