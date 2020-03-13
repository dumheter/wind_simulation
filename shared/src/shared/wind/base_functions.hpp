#pragma once

#include "shared/math/math.hpp"
#include "shared/types.hpp"
#include <variant>
#include <vector>

#include <ThirdParty/json.hpp>

namespace alflib {
class RawMemoryWriter;
class RawMemoryReader;
} // namespace alflib

namespace wind {

namespace baseFunctions {

enum class Type : u32 {
  kConstant = 0,
  kSpline,
};

String typeToString(Type type);

Type stringToType(const String& type);

// ============================================================ //

struct Constant {
  Vec3F dir;
  f32 magnitude;

  Vec3F operator()(const Vec3F) const { return dir * magnitude; }

  void toJson(nlohmann::json& value) const;
  static Constant fromJson(const nlohmann::json& value);

  bool ToBytes(alflib::RawMemoryWriter &mw) const;
  static Constant FromBytes(alflib::RawMemoryReader &mr);
};

struct Spline {
  std::vector<Vec3F> points;

  Vec3F operator()(const Vec3F point) const { return Vec3F::ZERO; }

  void toJson(nlohmann::json &value) const;
  static Spline fromJson(const nlohmann::json &value);

  bool ToBytes(alflib::RawMemoryWriter &mw) const;
  static Spline FromBytes(alflib::RawMemoryReader &mr);
};

} // namespace baseFunctions

// ============================================================ //

struct BaseFn {
  using Constant = baseFunctions::Constant;
  using Spline = baseFunctions::Spline;
  using Variant = std::variant<Constant, Spline>;

  Variant fn;

  template <typename T> bool isType() const {
    return std::holds_alternative<T>(fn);
  }

  static BaseFn fnConstant(Vec3F dir, f32 magnitude) {
    return BaseFn{Constant{dir, magnitude}};
  }

  /// Construct a BaseFn from json object.
  /// @param value Should be an object.
  /// ex:
  /// {
  ///   "type" : "constant",
  ///   "magnitude" : 20,
  ///   "direction" : [ 0.0, 1.0, 1.0 ]
  /// }
  static BaseFn fromJson(const nlohmann::json &value);

  /// Dump the BaseFn into a json representation.
  /// @param value Should be an (empty) object.
  /// ex:
  /// {}
  void toJson(nlohmann::json& value) const;

  /**
   * Calculate function value at point.
   */
  Vec3F operator()(Vec3F point) const {
    return std::visit([point](auto &&arg) { return arg(point); }, fn);
  }

  bool ToBytes(alflib::RawMemoryWriter &mw) const;

  static BaseFn FromBytes(alflib::RawMemoryReader &mr);

  String typeToString() const;

  // static String toString(baseFunctions::Type fn);


  // static BaseFn fromString(const String &fn);
};

// ============================================================ //

} // namespace wind
