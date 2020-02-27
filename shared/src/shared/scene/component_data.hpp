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

#include "shared/scene/types.hpp"
#include "shared/wind/base_functions.hpp"

#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

// ========================================================================== //
// ComponentData Declaration
// ========================================================================== //

namespace wind {

/// Union-like class with data about different components.
class ComponentData {

  /// Underlying enum type of the enum "ComponentType"
  using TagType = std::underlying_type_t<ComponentType>;

  /// Data associated with wind sources
  struct WindSourceData {
    std::vector<BaseFn> functions = {};
  };

  /// Data associated with rigidbodies
  struct RigidbodyData {
    f32 restitution = 0.0f;
    f32 mass = 0.0f;
  };

public:
  /// Returns the WindSource data
  const WindSourceData &windSourceData() const;

  /// Returns the Rigidbody data
  const RigidbodyData &rigidbodyData() const;

  /// Serializes to bytes
  bool ToBytes(alflib::RawMemoryWriter &mw) const;

  /// Deserializes from bytes
  static ComponentData FromBytes(alflib::RawMemoryReader &mr);

  /// Creates a component data representing wind source data
  static ComponentData asWindSource(const std::vector<BaseFn> &functions);

  /// Creates a component data representing rigidbody data
  static ComponentData asRigidbody(f32 restitution, f32 mass);

private:
  /// Variant
  std::variant<WindSourceData, RigidbodyData> m_data;
};

} // namespace wind