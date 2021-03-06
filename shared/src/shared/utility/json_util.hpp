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

#include "shared/macros.hpp"
#include "shared/math/math.hpp"
#include "shared/types.hpp"

#include <ThirdParty/json.hpp>

// ========================================================================== //
// JsonUtil Declaration
// ========================================================================== //

namespace wind {

///
class JsonUtil {
  WIND_NAMESPACE_CLASS(JsonUtil);

public:
  ///
  template <typename T, typename F, typename... ARGS>
  static T getOrCall(const nlohmann::json &value, const String &key, F &&func,
                     ARGS &&... arguments);

  static u32 getU32(const nlohmann::json &value, const String &key,
                    u32 fallback = 0);

  static f32 getF32(const nlohmann::json &value, const String &key,
                    f32 fallback = 0);

  /// Read a vector from a JSON value or return the default value
  static Vec2F getVec2F(const nlohmann::json &value, const String &key,
                        const Vec2F &fallback = Vec2F::ZERO);

  /// Read a vector from a JSON value or return the default value
  static Vec3F getVec3F(const nlohmann::json &value,
                        const Vec3F &fallback = Vec3F::ZERO);

  /// Read a vector from a JSON value or return the default value
  static Vec3F getVec3F(const nlohmann::json &value, const String &key,
                        const Vec3F &fallback = Vec3F::ZERO);

  /// Read a vector from a JSON value or return the default value
  static Vec4F getVec4F(const nlohmann::json &value, const String &key,
                        const Vec4F &fallback = Vec4F::ZERO);

  /// Read a string from a JSON value or return default value
  static String getString(const nlohmann::json &value, const String &key,
                          const String &fallback = "");

  /// Set Vec2
  static void setValue(nlohmann::json &value, const String &key,
                       const Vec2F &vec);

  /// Set Vec3
  static void setValue(nlohmann::json &value, const String &key,
                       const Vec3F &vec);
  /// Set Vec3
  static void setValue(nlohmann::json &value, const String &key,
                       const Vec4F &vec);

  /// Set Quat
  static void setValue(nlohmann::json &value, const String &key,
                       const Quat &quat);

  /// Create JSON from Vec3F.
  static nlohmann::json create(const Vec3F &vec);
};

// -------------------------------------------------------------------------- //

template <typename T, typename F, typename... ARGS>
T wind::JsonUtil::getOrCall(const nlohmann::json &value, const String &key,
                            F &&func, ARGS &&... args) {
  auto it = value.find(key.c_str());
  if (it != value.end()) {
    return *it;
  }
  return func(std::forward<ARGS>(args)...);
}

} // namespace wind
