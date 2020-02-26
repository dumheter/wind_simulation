#pragma once

#include "shared/types.hpp"
#include <Math/BsVector3.h>
#include <variant>
#include <vector>

namespace wind {

namespace baseFunctions {

struct Constant {
  bs::Vector3 dir;
  f32 magnitude;

  bs::Vector3 operator()(const bs::Vector3) const { return dir * magnitude; }
};

struct Spline {
  std::vector<bs::Vector3> points;

  bs::Vector3 operator()(const bs::Vector3 point) const {
    return bs::Vector3::ZERO;
  }
};

} // namespace baseFunctions

// ============================================================ //

struct BaseFn {
  using Constant = baseFunctions::Constant;
  using Spline = baseFunctions::Spline;

  using Variant = std::variant<Constant, Spline>;

  Variant fn;

  static BaseFn fnConstant(bs::Vector3 dir) {
    return BaseFn{Constant{dir, 1.0f}};
  }

  bs::Vector3 operator()(bs::Vector3 point) const {
    return std::visit([point](auto &&arg) { return arg(point); }, fn);
  }
};

// ============================================================ //

using BaseFnUnderlyingType = u32;

enum class BaseFnType : BaseFnUnderlyingType {
  kConstant,
  kSpline,
};

constexpr BaseFnType deserialize(BaseFnUnderlyingType value);

constexpr BaseFnUnderlyingType serialize(BaseFn fn);

} // namespace wind
