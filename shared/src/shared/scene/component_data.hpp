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

#include <variant>

#include "shared/scene/types.hpp"
#include "shared/types.hpp"
#include "shared/math/math.hpp"
#include "shared/wind/base_functions.hpp"

// ========================================================================== //
// ComponentData Declaration
// ========================================================================== //

namespace alflib {
	class RawMemoryReader;
	class RawMemoryWriter;
}

namespace wind {

/// Union-like class with data about different components.
class ComponentData {

public:
  /// Underlying enum type of the enum "ComponentType"
  using TagType = std::underlying_type_t<ComponentType>;

  struct RigidbodyData {};

  struct WindSourceData {
    std::vector<BaseFn> functions;
    Vec3F offset = Vec3F::ZERO;
    u8 volumeType = 0;
  };

  struct RenderableData {
    String pathTexture;
  };

  struct RotorData {
    Quat rot;
  };

  struct ColliderData {
    f32 restitution = 0.0f;
    f32 mass = 0.0f;
  };

  struct WindAffectableData {};

public:
  template <typename T> bool isType() const {
    return std::holds_alternative<T>(m_data);
  }

  /// Returns the Rigidbody data
  const RigidbodyData &rigidbodyData() const;

  /// Returns the WindSource data
  const WindSourceData &windSourceData() const;

  const RenderableData &renderableData() const;

  const RotorData &rotorData() const;

  const ColliderData &colliderData() const;

  const WindAffectableData &windAffectableData() const;

  /// Serializes to bytes
  bool ToBytes(alflib::RawMemoryWriter &mw) const;

  /// Deserializes from bytes
  static ComponentData FromBytes(alflib::RawMemoryReader &mr);

  /// Creates a component data representing rigidbody data
  static ComponentData asRigidbody();

  /// Creates a component data representing wind source data
  static ComponentData asWindSource(const std::vector<BaseFn> &functions,
                                    Vec3F offset, u8 volumeType);

  static ComponentData asRenderable(const String &pathTexture);

  static ComponentData asRotor(const Quat &rot);

  static ComponentData asCollider(f32 restitution, f32 mass);

  static ComponentData asWindAffectable();

private:
  /// Variant
  std::variant<RigidbodyData, WindSourceData, RenderableData, RotorData,
               ColliderData, WindAffectableData>
      m_data;
};

} // namespace wind
