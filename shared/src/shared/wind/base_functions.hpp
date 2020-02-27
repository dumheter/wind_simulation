#pragma once

#include "Math/BsVector3.h"
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
  bs::Vector3 dir;
  f32 magnitude;

  bs::Vector3 operator()(const bs::Vector3) const { return dir * magnitude; }

  bool ToBytes(alflib::RawMemoryWriter &mw) const;
  static Constant FromBytes(alflib::RawMemoryReader &mr);
};

struct Spline {
  std::vector<bs::Vector3> points;

  bs::Vector3 operator()(const bs::Vector3 point) const {
    return bs::Vector3::ZERO;
  }

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

  static BaseFn fnConstant(bs::Vector3 dir, f32 magnitude) {
    return BaseFn{Constant{dir, magnitude}};
  }

  bs::Vector3 operator()(bs::Vector3 point) const {
    return std::visit([point](auto &&arg) { return arg(point); }, fn);
  }

  bool ToBytes(alflib::RawMemoryWriter &mw) const;

  static BaseFn FromBytes(alflib::RawMemoryReader &mr);
};

// ============================================================ //

} // namespace wind