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
#include "shared/render/painter.hpp"
#include "shared/scene/rtti.hpp"

#include <functional>

#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// CPaint Declaration
// ========================================================================== //

namespace wind {

/// Class that represents a component which is used to implement paint
/// (primarily debug drawing) for a game object.
class CPaint : public bs::Component {
  friend class CTagRTTI;

public:
  using PainterFun = std::function<void(Painter &)>;

  /// Default constructor for serialization
  CPaint();

  ~CPaint() override;

  /// Construct a paint component.
  explicit CPaint(const bs::HSceneObject &parent);

  /// Callback to paint.
  virtual void paint(Painter &painter) { m_paintCallback(painter); }

  /// Set the painter callback function.
  void setPaintCallback(PainterFun function) { m_paintCallback = function; }

  /// Returns a reference to the static RTTI object that represents this
  /// component
  static bs::RTTITypeBase *getRTTIStatic();

  /// \copydoc bs::IReflectable::getRTTI
  bs::RTTITypeBase *getRTTI() const override { return getRTTIStatic(); }

  /// Returns all instances of component.
  static const std::vector<CPaint *> &getAll() { return s_Components; }

private:
  /// List of all created components of this type.
  static std::vector<CPaint *> s_Components;

  /// Optional paint callback for user.
  PainterFun m_paintCallback;
};

// -------------------------------------------------------------------------- //

/// CPaint handle type
using HCPaint = bs::GameObjectHandle<CPaint>;

} // namespace wind

// ========================================================================== //
// CPaintRTTI Declaration
// ========================================================================== //

namespace wind {

/// CPaint component RTTI
class CPaintRTTI : public bs::RTTIType<CPaint, bs::Component, CPaintRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_END_RTTI_MEMBERS

public:
  /// \copydoc bs::RTTITypeBase::getRTTIName
  const bs::String &getRTTIName() override {
    static bs::String name = "CPaint";
    return name;
  }

  /// \copydoc bs::RTTITypeBase::getRTTIId
  bs::UINT32 getRTTIId() override { return TID_CPaint; }

  /// \copydoc bs::RTTITypeBase::newRTTIObject
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CPaint>();
  }
};

} // namespace wind