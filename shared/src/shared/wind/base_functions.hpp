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
  kSplineCollection,
};

String typeToString(Type type);

Type stringToType(const String &type);

// ============================================================ //

struct Constant {
  Vec3F dir;
  f32 magnitude;

  Vec3F operator()(const Vec3F) const { return dir * magnitude; }

  void toJson(nlohmann::json &value) const;
  static Constant fromJson(const nlohmann::json &value);

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

struct SplineCollection;
struct Spline {
  friend struct SplineCollection;
  std::vector<Vec3F> points;
  u32 degree;
  u32 samples;

  Vec3F operator()(Vec3F point) const;

  void toJson(nlohmann::json &value) const;
  static Spline fromJson(const nlohmann::json &value);

  bool ToBytes(alflib::RawMemoryWriter &mw) const;
  static Spline FromBytes(alflib::RawMemoryReader &mr);

private:
  struct ClosestPoint {
    u32 index;
    f32 distance;
  };
  ClosestPoint findClosestPoint(Vec3F point) const;

  /// @param point Point where we want to know wind force
  /// @param closestPoint
  /// @param dist Distance from point to closestPoint
  Vec3F getForce(Vec3F point, ClosestPoint closestPoint) const;
};

struct SplineCollection {
  std::vector<Spline> splines;

  Vec3F operator()(Vec3F point) const;

  void toJson(nlohmann::json &value) const;
  static SplineCollection fromJson(const nlohmann::json &value);

  bool ToBytes(alflib::RawMemoryWriter &mw) const;
  static SplineCollection FromBytes(alflib::RawMemoryReader &mr);
};

} // namespace baseFunctions

// ============================================================ //

struct BaseFn {
  using Constant = baseFunctions::Constant;
  using Polynomial = baseFunctions::Polynomial;
  using Spline = baseFunctions::Spline;
  using SplineCollection = baseFunctions::SplineCollection;
  using Variant = std::variant<Constant, Polynomial, Spline, SplineCollection>;

  Variant fn;

  template <typename T> bool isType() const {
    return std::holds_alternative<T>(fn);
  }

  static BaseFn fnConstant(Vec3F dir, f32 magnitude) {
    return BaseFn{Constant{dir, magnitude}};
  }

  static BaseFn fnPolynomial(Vec3F origo, f32 x0, f32 x1, f32 x2, f32 y0,
                             f32 y1, f32 y2, f32 z0, f32 z1, f32 z2) {
    return BaseFn{Polynomial{origo, x0, x1, x2, y0, y1, y2, z0, z1, z2}};
  }

  static BaseFn fnSpline(std::vector<Vec3F> &&points, u32 degree, u32 samples) {
    return BaseFn{Spline{std::move(points), degree, samples}};
  }

  static BaseFn fnSplineCollection(std::vector<Spline> &&splines) {
    return BaseFn{SplineCollection{std::move(splines)}};
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
  void toJson(nlohmann::json &value) const;

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
