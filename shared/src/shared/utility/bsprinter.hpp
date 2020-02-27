/**
 * This file defines a way for fmt to format and print both
 * bs::Vector3 and bs::Quaternion.
 */

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/log.hpp"
#include "shared/math/math.hpp"
#include "shared/scene/component/ctag.hpp"

#include <ostream>

#include <thirdparty/fmt/ostream.h>

#include <Math/BsQuaternion.h>
#include <Math/BsVector3.h>

// ========================================================================== //
// Functions
// ========================================================================== //

std::ostream &operator<<(std::ostream &out, const bs::Vector3 &vec);

// -------------------------------------------------------------------------- //

std::ostream &operator<<(std::ostream &out, const bs::Vector2 &vec);

// -------------------------------------------------------------------------- //

std::ostream &operator<<(std::ostream &out, wind::ObjectType type);

// -------------------------------------------------------------------------- //

std::ostream &operator<<(std::ostream &out, const bs::HComponent &comp);

// ========================================================================== //
// Structures
// ========================================================================== //

template <> struct fmt::formatter<bs::Vector3> {

  char presentation = 'f';

  constexpr auto parse(format_parse_context &ctx) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e'))
      presentation = *it++;

    if (it != end && *it != '}')
      throw format_error("invalid format");

    return it;
  }

  template <typename FormatContext>
  auto format(const bs::Vector3 &v, FormatContext &ctx) {
    return format_to(ctx.out(),
                     presentation == 'f' ? "({:.1f}, {:.1f}, {:.1f})"
                                         : "({:.1e}, {:.1e}, {:.1e})",
                     v.x, v.y, v.z);
  }
};

// -------------------------------------------------------------------------- //

template <> struct fmt::formatter<bs::Quaternion> {

  char presentation = 'f';

  constexpr auto parse(format_parse_context &ctx) {
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e'))
      presentation = *it++;

    if (it != end && *it != '}')
      throw format_error("invalid format");

    return it;
  }

  template <typename FormatContext>
  auto format(const bs::Quaternion &v, FormatContext &ctx) {
    return format_to(ctx.out(),
                     presentation == 'f' ? "({:.1f}, {:.1f}, {:.1f}, {:.1f})"
                                         : "({:.1e}, {:.1e}, {:.1e}, {:.1e})",
                     v.x, v.y, v.z, v.w);
  }
};
