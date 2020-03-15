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
#include "shared/scene/component/cpaint.hpp"
#include "shared/scene/rtti.hpp"
#include "shared/sim/wind_sim.hpp"

#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>

// ========================================================================== //
// CSim Declaration
// ========================================================================== //

namespace wind {

/// Class that represents a component which handles simulation of wind.
class CSim : public CPaint {
  friend class CTagRTTI;

public:
  /// Construct simulation component.
  CSim() = default;

  /// Construct a simulation component.
  explicit CSim(const bs::HSceneObject &parent);

  /// Build the simualation for a specific scene. Nullptr can be passed to build
  /// for default scene.
  void build(s32 width, s32 height, s32 depth, f32 cellSize = 1.0f,
             const bs::SPtr<bs::SceneInstance> &scene = nullptr);

  /// Build the simualation for a specific scene. Nullptr can be passed to build
  /// for default scene.
  void build(const Vec3I &dim, f32 cellSize = 1.0f,
             const bs::SPtr<bs::SceneInstance> &scene = nullptr) {
    build(dim.x, dim.y, dim.z, cellSize, scene);
  }

  /// Bake the simulation out into an object that represents the individual
  /// objects generated.
  bs::HSceneObject bake();

  /// Returns a pointer to the simulation object.
  WindSimulation *getSim() const { return m_sim; }

  /// Paint simulation
  void paint(Painter &painter) override;

  /// Fixed update
  void fixedUpdate() override;

  /// Returns a reference to the static RTTI object that represents this
  /// component
  static bs::RTTITypeBase *getRTTIStatic();

  /// \copydoc bs::IReflectable::getRTTI
  bs::RTTITypeBase *getRTTI() const override { return getRTTIStatic(); }

private:
  /// Simulation
  WindSimulation *m_sim = nullptr;
};

// -------------------------------------------------------------------------- //

/// CSim handle type
using HCSim = bs::GameObjectHandle<CSim>;

} // namespace wind

// ========================================================================== //
// CSimRTTI Declaration
// ========================================================================== //

namespace wind {

/// CSim component RTTI
class CSimRTTI : public bs::RTTIType<CSim, bs::Component, CSimRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_END_RTTI_MEMBERS

public:
  /// \copydoc bs::RTTITypeBase::getRTTIName
  const bs::String &getRTTIName() override {
    static bs::String name = "CSim";
    return name;
  }

  /// \copydoc bs::RTTITypeBase::getRTTIId
  bs::UINT32 getRTTIId() override { return TID_CSim; }

  /// \copydoc bs::RTTITypeBase::newRTTIObject
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CSim>();
  }
};

} // namespace wind