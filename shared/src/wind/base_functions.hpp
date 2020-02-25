#pragma once

#include "Math/BsVector3.h"
#include "types.hpp"
#include <variant>
#include <vector>

namespace wind {

struct FnConstant {
  bs::Vector3 dir;

  bs::Vector3 operator()(bs::Vector3 _) const { return dir; }
};

struct FnSpline {
  std::vector<bs::Vector3> points;

  bs::Vector3 operator()(bs::Vector3 point) const { return bs::Vector3::ZERO; }
};

// ============================================================ //

struct BaseFn {
  using Variant = std::variant<FnConstant, FnSpline>;
  Variant fn;

  static BaseFn fnConstant(bs::Vector3 dir) { return BaseFn{FnConstant{dir}}; }

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
