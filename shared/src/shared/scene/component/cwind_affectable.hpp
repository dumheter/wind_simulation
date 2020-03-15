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

#include <RTTI/BsMathRTTI.h>
#include <Reflection/BsRTTIPlain.h>
#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>

#include "shared/scene/rtti.hpp"

// ========================================================================== //
// CWindAffectable Declaration
// ========================================================================== //

namespace wind {

/// A component that will make the object be affected by wind.
/// It querys the global wind system, and applies any wind force.
class CWindAffectable : public bs::Component {
  friend class CWindAffectableRTTI;

public:
  /// Default constructor required for serialization
  CWindAffectable() = default;

  CWindAffectable(const bs::HSceneObject &parent);

  void fixedUpdate() override;

  static bs::RTTITypeBase *getRTTIStatic();

  bs::RTTITypeBase *getRTTI() const override;
};

} // namespace wind

// ========================================================================== //
// CWindAffectableRTTI Declaration
// ========================================================================== //
namespace wind {

/// CWindAffectable RTTI
class CWindAffectableRTTI
    : public bs::RTTIType<CWindAffectable, bs::Component, CWindAffectableRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_END_RTTI_MEMBERS

public:
  const bs::String &getRTTIName() override {
    static bs::String name = "CWindAffectable";
    return name;
  }

  bs::UINT32 getRTTIId() override { return TID_CWindAffectable; }

  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CWindAffectable>();
  }
};

} // namespace wind