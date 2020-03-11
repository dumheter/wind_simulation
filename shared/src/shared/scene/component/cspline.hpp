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

/// Class that represents a component which is used to tag objects with extra
/// information. Currently the tag contains only the type of the object
class CSpline final : public bs::Component {
  friend class CTagRTTI;

  struct Data;

public:
  /// Default constructor for serialization
  CSpline() = default;

  /// Construct a spline component.
  CSpline(const bs::HSceneObject &parent, const Spline &spline);

  /// Construct a spline component.
  CSpline(const bs::HSceneObject &parent, const std::vector<Vec3F> &points,
          u32 degrees = 2);

  /// Returns the underlying spline data
  Spline *getSpline() { return m_spline; }

  /// Returns the underlying spline data
  const Spline *getSpline() const { return m_spline; }

  /// Returns a reference to the static RTTI object that represents this
  /// component
  static bs::RTTITypeBase *getRTTIStatic();

  /// \copydoc bs::IReflectable::getRTTI
  bs::RTTITypeBase *getRTTI() const override { return getRTTIStatic(); }

private:
  /// Spline
  Spline *m_spline = nullptr;
};

// -------------------------------------------------------------------------- //

/// CSpline handle type
using HCSpline = bs::GameObjectHandle<CSpline>;

} // namespace wind

// ========================================================================== //
// CSplineRTTI Declaration
// ========================================================================== //

namespace wind {

/// CSpline component RTTI
class CSplineRTTI : public bs::RTTIType<CSpline, bs::Component, CSplineRTTI> {
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
  bs::UINT32 getRTTIId() override { return TID_CSpline; }

  /// \copydoc bs::RTTITypeBase::newRTTIObject
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CSpline>();
  }
};

} // namespace wind