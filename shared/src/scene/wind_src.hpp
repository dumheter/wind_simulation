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

#include "scene/rtti.hpp"

#include <BsPrerequisites.h>
#include <RTTI/BsMathRTTI.h>
#include <Reflection/BsRTTIPlain.h>
#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// CWindSource Declaration
// ========================================================================== //

namespace wind {

///
class CWindSource : public bs::Component {
  friend class CWindSourceRTTI;

public:
  /// Default constructor required for serialization
  CWindSource() = default;

  explicit CWindSource(const bs::HSceneObject &parent);

  /// Construct wind source from list of basic functions
  // CWindSource(const std::vector<BasicFunction>& functions);

  /// \copydoc bs::Component::onCreated()
  void onCreated() override;

  /// \copydoc bs::Component::onTransformChanged()
  void onTransformChanged(bs::TransformChangedFlags flags) override;

  ///
  static bs::RTTITypeBase *getRTTIStatic();

  bs::RTTITypeBase *getRTTI() const override { return getRTTIStatic(); }

private:
  /// List of basic functions
  // std::vector<BasicFunction> m_functions;
};

// -------------------------------------------------------------------------- //

/// CWindSource handle type
using HCWindSource = bs::GameObjectHandle<CWindSource>;

} // namespace wind

// ========================================================================== //
// CWindSourceRTTI
// ========================================================================== //

namespace wind {

///
class CWindSourceRTTI
    : public bs::RTTIType<CWindSource, bs::Component, CWindSourceRTTI> {
public:
  const bs::String &getRTTIName() override { return s_name; }

  bs::UINT32 getRTTIId() override { return TID_CWindSource; }

  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CWindSource>();
  }

private:
  /// Static resource type name
  static bs::String s_name;
};

} // namespace wind