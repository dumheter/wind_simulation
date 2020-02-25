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

wind::Vec2F JsonUtil::getVec2F(const nlohmann::json &value, const String &key,
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
    } else if (obj.is_object()) {
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
    } else {
      return fallback;
    }
  }
  return fallback;
}

// -------------------------------------------------------------------------- //

wind::Vec3F JsonUtil::getVec3F(const nlohmann::json &value, const String &key,
                               const Vec3F &fallback) {
  auto it = value.find(key.c_str());
  if (it != value.end()) {
    auto obj = *it;
    if (obj.is_array()) {
      size_t count = obj.size();
      f32 x = fallback.x;
      f32 y = fallback.y;
      f32 z = fallback.z;
      if (count >= 1) {
        x = obj[0];
      }
      if (count >= 2) {
        y = obj[1];
      }
      if (count >= 3) {
        z = obj[2];
      }
      return Vec3F(x, y, z);
    } else if (obj.is_object()) {
      f32 x = obj.value("x", fallback.x);
      f32 y = obj.value("y", fallback.y);
      f32 z = obj.value("z", fallback.z);

      auto _it = obj.find("xyz");
      if (_it != obj.end()) {
        x = y = z = obj["xyz"];
      }
      _it = obj.find("xy");
      if (_it != obj.end()) {
        x = y = obj["xy"];
      }
      _it = obj.find("xz");
      if (_it != obj.end()) {
        x = z = obj["xz"];
      }
      _it = obj.find("yz");
      if (_it != obj.end()) {
        y = z = obj["yz"];
      }

      return Vec3F(x, y, z);
    } else {
      return fallback;
    }
  }
  return fallback;
}

} // namespace wind