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

#include "shared/math/math.hpp"
#include "shared/types.hpp"

// ========================================================================== //
// WindSystem Declaration
// ========================================================================== //

namespace wind {

/// Singleton wind system
class WindSystem {
public:
  /// Physics layer for wind sources
  static constexpr u64 kWindVolumeLayer = 1024;
  /// Physics layer for wind occluders
  static constexpr u64 kWindOccluerLayer = 1025;

  /// Enumeration of volume types
  enum class VolumeType {
    kCube = 0,     ///< Cube volume.
    kCylinder, ///< Cylinder volume.
  };

  static String volumeTypeToString(VolumeType type);

  static VolumeType stringToVolumeType(const String &type);

  static u8 volumeTypeToU8(VolumeType type);

  static VolumeType u8ToVolumeType(u8 type);

  // ============================================================ //

 public:
  /// @note Can also access via gWindSystem()
  static WindSystem &instance();

  Vec3F getWindAtPoint(Vec3F point) const;

 private:
  WindSystem() = default;

  WindSystem(const WindSystem& other) = delete;
  WindSystem& operator=(const WindSystem &other) = delete;
};

inline WindSystem &gWindSystem() { return WindSystem::instance(); }

} // namespace wind
