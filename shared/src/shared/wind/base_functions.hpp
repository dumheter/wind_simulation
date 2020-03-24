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
  kPolynomial,
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

struct Polynomial {
  Vec3F origo;
  f32 x0;
  f32 x1;
  f32 x2;
  f32 y0;
  f32 y1;
  f32 y2;
  f32 z0;
  f32 z1;
  f32 z2;

  Vec3F operator()(const Vec3F point) const {
    const Vec3F p = point - origo;
    return Vec3F{(x0) + (x1 * p.x) + (x2 * p.x * p.x),
                 (y0) + (y1 * p.y) + (y2 * p.y * p.y),
                 (z0) + (z1 * p.z) + (z2 * p.z * p.z)};
  }

  void toJson(nlohmann::json &value) const;
  static Polynomial fromJson(const nlohmann::json &value);

  bool ToBytes(alflib::RawMemoryWriter &mw) const;
  static Polynomial FromBytes(alflib::RawMemoryReader &mr);
};

struct Spline {
  std::vector<Vec3F> points;
  u32 degree;
  u32 samples;

  Vec3F operator()(Vec3F point) const;

  void toJson(nlohmann::json &value) const;
  static Spline fromJson(const nlohmann::json &value);

  bool ToBytes(alflib::RawMemoryWriter &mw) const;
  static Spline FromBytes(alflib::RawMemoryReader &mr);
};

} // namespace baseFunctions

// ============================================================ //

struct BaseFn {
  using Constant = baseFunctions::Constant;
  using Polynomial = baseFunctions::Polynomial;
  using Spline = baseFunctions::Spline;
  using Variant = std::variant<Constant, Polynomial, Spline>;

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

  /// Calculate function value at point.
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
