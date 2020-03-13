// MIT License
//
// Copyright (c) 2020 Filip Björklund, Christoffer Gustafsson
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

#include "shared/math/math.hpp"
#include "shared/scene/rtti.hpp"

#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// CWindOccluder Declaration
// ========================================================================== //

namespace wind {

/// Class that represents a component which is used to represend an occluder for
/// the wind system.
class CWindOccluder final : public bs::Component {
  friend class CTagRTTI;

public:
  /// Occluder volume types.
  enum class VolumeKind {
    kCube,     ///< Cube volume.
    kCylinder, ///< Cylinder volume.
  };

  /// Cube description.
  struct Cube {
    Vec3F dim; ///< Dimension of cube (x, y and z radii)
  };

  /// Cylinder description
  struct Cylinder {
    f32 height; ///< Cylinder height.
    f32 radius; ///< Cylinder radius.
  };

  /// Default constructor for serialization
  CWindOccluder() = default;

  /// Construct a wind occluder component.
  CWindOccluder(const bs::HSceneObject &parent, const Cube &cube);

  /// Construct a wind occluder component.
  CWindOccluder(const bs::HSceneObject &parent, const Cylinder &cylinder);

  /// Returns a reference to the static RTTI object that represents this
  /// component
  static bs::RTTITypeBase *getRTTIStatic();

  /// \copydoc bs::IReflectable::getRTTI
  bs::RTTITypeBase *getRTTI() const override { return getRTTIStatic(); }

private:
  /// Volume Kind
  VolumeKind m_volumeKind = VolumeKind::kCube;
};

// -------------------------------------------------------------------------- //

/// CWindOccluder handle type
using HCWindOccluder = bs::GameObjectHandle<CWindOccluder>;

} // namespace wind

// ========================================================================== //
// CWindOccluderRTTI Declaration
// ========================================================================== //

namespace wind {

/// CWindOccluder component RTTI
class CWindOccluderRTTI
    : public bs::RTTIType<CWindOccluder, bs::Component, CWindOccluderRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_END_RTTI_MEMBERS

public:
  /// \copydoc bs::RTTITypeBase::getRTTIName
  const bs::String &getRTTIName() override {
    static bs::String name = "CSpline";
    return name;
  }

  /// \copydoc bs::RTTITypeBase::getRTTIId
  bs::UINT32 getRTTIId() override { return TID_CWindOccluder; }

  /// \copydoc bs::RTTITypeBase::newRTTIObject
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CWindOccluder>();
  }
};

} // namespace wind