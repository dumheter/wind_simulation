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
#include "shared/math/spline.hpp"
#include "shared/scene/rtti.hpp"

#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// CSpline Declaration
// ========================================================================== //

namespace wind {

/// Class that represents a component which is used for objects to follow a
/// spline. This component assumes that there is also a 'CSpline' component on
/// the object
class CSplineFollow final : public bs::Component {
  friend class CTagRTTI;

public:
  /// Default constructor for serialization
  CSplineFollow() = default;

  /// Construct a spline-follow component.
  CSplineFollow(const bs::HSceneObject &parent);

  /// Returns a reference to the static RTTI object that represents this
  /// component
  static bs::RTTITypeBase *getRTTIStatic();

  /// \copydoc bs::IReflectable::getRTTI
  bs::RTTITypeBase *getRTTI() const override { return getRTTIStatic(); }
};

// -------------------------------------------------------------------------- //

/// CSplineFollow handle type
using HCSplineFollow = bs::GameObjectHandle<CSplineFollow>;

} // namespace wind

// ========================================================================== //
// CSplineRTTI Declaration
// ========================================================================== //

namespace wind {

/// CSplineFollow component RTTI
class CSplineFollowRTTI
    : public bs::RTTIType<CSplineFollow, bs::Component, CSplineFollowRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_END_RTTI_MEMBERS

public:
  /// \copydoc bs::RTTITypeBase::getRTTIName
  const bs::String &getRTTIName() override {
    static bs::String name = "CSplineFollow";
    return name;
  }

  /// \copydoc bs::RTTITypeBase::getRTTIId
  bs::UINT32 getRTTIId() override { return TID_CSpline; }

  /// \copydoc bs::RTTITypeBase::newRTTIObject
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CSplineFollow>();
  }
};

} // namespace wind