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

#include "shared/utility/json_util.hpp"

// ========================================================================== //
// JsonUtil Implementation
// ========================================================================== //

namespace wind {

u32 JsonUtil::getU32(const nlohmann::json &value, const String &key,
                     u32 fallback) {
  const auto it = value.find(key.c_str());
  if (it != value.end()) {
    return value[key.c_str()];
  }
  return fallback;
}

// -------------------------------------------------------------------------- //

f32 JsonUtil::getF32(const nlohmann::json &value, const String &key,
                     f32 fallback) {
  const auto it = value.find(key.c_str());
  if (it != value.end()) {
    return value[key.c_str()];
  }
  return fallback;
}

// -------------------------------------------------------------------------- //

Vec2F JsonUtil::getVec2F(const nlohmann::json &value, const String &key,
                         const Vec2F &fallback) {
  auto it = value.find(key.c_str());
  if (it != value.end()) {
    auto obj = *it;
    if (obj.is_array()) {
      size_t count = obj.size();
      f32 x = fallback.x;
      f32 y = fallback.y;
      if (count >= 1) {
        x = obj[0];
      }
      if (count >= 2) {
        y = obj[1];
      }
      return Vec2F(x, y);
    }
    if (obj.is_object()) {
      f32 x = obj.value("x", fallback.x);
      f32 y = obj.value("y", fallback.y);
      x = obj.value("u", x);
      y = obj.value("v", y);
      auto _it = obj.find("xy");
      if (_it != obj.end()) {
        x = y = obj["xy"];
      }
      _it = obj.find("uv");
      if (_it != obj.end()) {
        x = y = obj["uv"];
      }
      return Vec2F(x, y);
    }
    return fallback;
  }
  return fallback;
}

// -------------------------------------------------------------------------- //

Vec3F JsonUtil::getVec3F(const nlohmann::json &value, const Vec3F &fallback) {
  if (value.is_array()) {
    const size_t count = value.size();
    f32 x = fallback.x;
    f32 y = fallback.y;
    f32 z = fallback.z;
    if (count >= 1) {
      x = value[0];
    }
    if (count >= 2) {
      y = value[1];
    }
    if (count >= 3) {
      z = value[2];
    }
    return Vec3F(x, y, z);
  }
  if (value.is_object()) {
    f32 x = value.value("x", fallback.x);
    f32 y = value.value("y", fallback.y);
    f32 z = value.value("z", fallback.z);

    auto _it = value.find("xyz");
    if (_it != value.end()) {
      x = y = z = value["xyz"];
    }
    _it = value.find("xy");
    if (_it != value.end()) {
      x = y = value["xy"];
    }
    _it = value.find("xz");
    if (_it != value.end()) {
      x = z = value["xz"];
    }
    _it = value.find("yz");
    if (_it != value.end()) {
      y = z = value["yz"];
    }

    return Vec3F(x, y, z);
  }
  return fallback;
}

// -------------------------------------------------------------------------- //

Vec3F JsonUtil::getVec3F(const nlohmann::json &value, const String &key,
                         const Vec3F &fallback) {
  const auto it = value.find(key.c_str());
  if (it != value.end()) {
    return getVec3F(*it, fallback);
  }
  return fallback;
}

// -------------------------------------------------------------------------- //

Vec4F JsonUtil::getVec4F(const nlohmann::json &value, const String &key,
                         const Vec4F &fallback) {
  auto it = value.find(key.c_str());
  if (it != value.end()) {
    auto obj = *it;
    if (obj.is_array()) {
      const size_t count = obj.size();
      f32 x = fallback.x;
      f32 y = fallback.y;
      f32 z = fallback.z;
      f32 w = fallback.w;
      if (count >= 1) {
        x = obj[0];
      }
      if (count >= 2) {
        y = obj[1];
      }
      if (count >= 3) {
        z = obj[2];
      }
      if (count >= 4) {
        w = obj[3];
      }
      return Vec4F(x, y, z, w);
    }
    if (obj.is_object()) {
      const f32 x = obj.value("x", fallback.x);
      const f32 y = obj.value("y", fallback.y);
      const f32 z = obj.value("z", fallback.z);
      const f32 w = obj.value("w", fallback.w);
      return Vec4F(x, y, z, w);
    }
    return fallback;
  }
  return fallback;
}

// -------------------------------------------------------------------------- //

String JsonUtil::getString(const nlohmann::json &value, const String &key,
                           const String &fallback) {
  return value.value(key.c_str(), fallback.c_str()).c_str();
}

// -------------------------------------------------------------------------- //

void JsonUtil::setValue(nlohmann::json &value, const String &key,
                        const Vec2F &vec) {
  value[key.c_str()]["x"] = vec.x;
  value[key.c_str()]["y"] = vec.y;
}

// -------------------------------------------------------------------------- //

void JsonUtil::setValue(nlohmann::json &value, const String &key,
                        const Vec3F &vec) {
  value[key.c_str()]["x"] = vec.x;
  value[key.c_str()]["y"] = vec.y;
  value[key.c_str()]["z"] = vec.z;
}

void JsonUtil::setValue(nlohmann::json &value, const String &key,
                        const Vec4F &vec) {
  value[key.c_str()]["x"] = vec.x;
  value[key.c_str()]["y"] = vec.y;
  value[key.c_str()]["z"] = vec.z;
  value[key.c_str()]["w"] = vec.w;
}

// -------------------------------------------------------------------------- //

void JsonUtil::setValue(nlohmann::json &value, const String &key,
                        const Quat &quat) {
  value[key.c_str()]["x"] = quat.x;
  value[key.c_str()]["y"] = quat.y;
  value[key.c_str()]["z"] = quat.z;
}

// -------------------------------------------------------------------------- //

nlohmann::json JsonUtil::create(const Vec3F &vec) {
  nlohmann::json value;
  value["x"] = vec.x;
  value["y"] = vec.y;
  value["z"] = vec.z;
  return value;
}

} // namespace wind
