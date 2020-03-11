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

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/math/math.hpp"
#include "shared/scene/rtti.hpp"
#include "shared/scene/types.hpp"
#include "shared/utility/unique_id.hpp"

#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>
#include <optional>

// ========================================================================== //
// CTag Declaration
// ========================================================================== //

namespace wind {

/// Class that represents a component which is used to tag objects with extra
/// information. Currently the tag contains only the type of the object
class CTag final : public bs::Component {
  friend class CTagRTTI;

  struct Data;

public:
  /// Default constructor for serialization
  CTag() = default;

  /// Construct a tag component with a specific object type as the tag.
  CTag(const bs::HSceneObject &parent, ObjectType type);

  /// Returns the type of the tag
  ObjectType getType() const { return m_type; }

  /// Returns the reference to the additional data stored in the tag component.
  Data &getData() { return m_data; }

  /// Returns the reference to the additional data stored in the tag component.
  const Data &getData() const { return m_data; }

  /// Returns a reference to the static RTTI object that represents this
  /// component
  static bs::RTTITypeBase *getRTTIStatic();

  /// \copydoc bs::IReflectable::getRTTI
  bs::RTTITypeBase *getRTTI() const override { return getRTTIStatic(); }

private:
  /// Type of the tagged object
  ObjectType m_type = ObjectType::kInvalid;

  /// Additional data required to be stored for objects
  struct Data {
    bool save = true; ///< Whether object should be saved
    String skybox = "";
    struct Material {
      String shader = "standard";
      String albedo = "";
      Vec2F tiling = Vec2F::ONE;
      Vec4F color = Vec4F(1.0f, 1.0f, 1.0f, 1.0f);
    } mat;
    struct Spline {
      std::vector<Vec3F> points = {};
    };
    struct Physics {
      f32 restitution = 0.5f;
      f32 mass = 10.0f;
      bool collider = false;
      bool rigidbody = false;
    } physics;
    std::optional<UniqueId> id = std::nullopt;
  } m_data;
};

// -------------------------------------------------------------------------- //

/// CTag handle type
using HCTag = bs::GameObjectHandle<CTag>;

} // namespace wind

// ========================================================================== //
// CTagRTTI Declaration
// ========================================================================== //

namespace wind {

/// CTag component RTTI
class CTagRTTI : public bs::RTTIType<CTag, bs::Component, CTagRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_END_RTTI_MEMBERS

public:
  /// \copydoc bs::RTTITypeBase::getRTTIName
  const bs::String &getRTTIName() override {
    static bs::String name = "CTag";
    return name;
  }

  /// \copydoc bs::RTTITypeBase::getRTTIId
  bs::UINT32 getRTTIId() override { return TID_CTag; }

  /// \copydoc bs::RTTITypeBase::newRTTIObject
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CTag>();
  }
};

} // namespace wind
