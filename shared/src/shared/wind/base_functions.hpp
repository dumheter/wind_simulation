#pragma once

#include "shared/math/math.hpp"
#include "shared/types.hpp"
#include <variant>
#include <vector>

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

struct Constant {
  Vec3F dir;
  f32 magnitude;

  Vec3F operator()(const Vec3F) const { return dir * magnitude; }

  bool ToBytes(alflib::RawMemoryWriter &mw) const;
  static Constant FromBytes(alflib::RawMemoryReader &mr);
};

struct Spline {
  std::vector<Vec3F> points;

  Vec3F operator()(const Vec3F point) const { return Vec3F::ZERO; }

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

  static BaseFn fnConstant(Vec3F dir, f32 magnitude) {
    return BaseFn{Constant{dir, magnitude}};
  }

  Vec3F operator()(Vec3F point) const {
    return std::visit([point](auto &&arg) { return arg(point); }, fn);
  }

  bool ToBytes(alflib::RawMemoryWriter &mw) const;

  static BaseFn FromBytes(alflib::RawMemoryReader &mr);

  String toString(baseFunctions::Type fn) const;

  static BaseFn fromString(const String &fn);
};

// ============================================================ //

} // namespace wind
