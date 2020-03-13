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

#include "component_data.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/log.hpp"
#include "shared/utility/util.hpp"
#include "shared/wind/base_functions.hpp"

// ========================================================================== //
// ComponentData Implementation
// ========================================================================== //

namespace wind {

const ComponentData::RigidbodyData &ComponentData::rigidbodyData() const {
  return std::get<RigidbodyData>(m_data);
}

// -------------------------------------------------------------------------- //

const ComponentData::WindData &ComponentData::windSourceData() const {
  return std::get<WindData>(m_data);
}

// -------------------------------------------------------------------------- //

const ComponentData::RenderableData &ComponentData::renderableData() const {
  return std::get<RenderableData>(m_data);
}

// -------------------------------------------------------------------------- //

const ComponentData::RotorData &ComponentData::rotorData() const {
  return std::get<RotorData>(m_data);
}

// -------------------------------------------------------------------------- //

const ComponentData::ColliderData &ComponentData::colliderData() const {
  return std::get<ColliderData>(m_data);
}

const ComponentData::WindAffectableData &ComponentData::windAffectableData() const {
  return std::get<WindAffectableData>(m_data);
}

// -------------------------------------------------------------------------- //

bool ComponentData::ToBytes(alflib::RawMemoryWriter &mw) const {
  if (std::holds_alternative<RigidbodyData>(m_data)) {
    return mw.Write(static_cast<TagType>(ComponentType::kRigidbody));
  } else if (std::holds_alternative<WindData>(m_data)) {
    mw.Write(static_cast<TagType>(ComponentType::kWindSource));
    mw.Write(std::get<WindData>(m_data).functions);
    mw.Write(std::get<WindData>(m_data).offset.x);
    mw.Write(std::get<WindData>(m_data).offset.y);
    mw.Write(std::get<WindData>(m_data).offset.z);
    return mw.Write(std::get<WindData>(m_data).volumeType);
  } else if (std::holds_alternative<RenderableData>(m_data)) {
    mw.Write(static_cast<TagType>(ComponentType::kRenderable));
    return mw.Write(
        alflib::String(std::get<RenderableData>(m_data).pathTexture.c_str()));
  } else if (std::holds_alternative<RotorData>(m_data)) {
    mw.Write(static_cast<TagType>(ComponentType::kRotor));
    mw.Write(std::get<RotorData>(m_data).rot.x);
    mw.Write(std::get<RotorData>(m_data).rot.y);
    mw.Write(std::get<RotorData>(m_data).rot.z);
    return mw.Write(std::get<RotorData>(m_data).rot.w);
  } else if (std::holds_alternative<ColliderData>(m_data)) {
    mw.Write(static_cast<TagType>(ComponentType::kCollider));
    mw.Write(std::get<ColliderData>(m_data).restitution);
    return mw.Write(std::get<ColliderData>(m_data).mass);
  } else if (std::holds_alternative<WindAffectableData>(m_data)) {
    return mw.Write(static_cast<TagType>(ComponentType::kWindAffectable));
  } else {
    Util::panic("Invalid ComponentData variant");
  }
}

// -------------------------------------------------------------------------- //

ComponentData ComponentData::FromBytes(alflib::RawMemoryReader &mr) {

  // Initialize per type
  ComponentType type = static_cast<ComponentType>(mr.Read<TagType>());
  switch (type) {
  case ComponentType::kRigidbody: {
    return asRigidbody();
  }
  case ComponentType::kWindSource: {
    std::vector<BaseFn> functions = mr.ReadStdVector<BaseFn>();
    auto offset = Vec3F{mr.Read<f32>(), mr.Read<f32>(), mr.Read<f32>()};
    auto volumeType = mr.Read<u8>();
    return asWind(functions, offset, volumeType);
  }
  case ComponentType::kRenderable: {
    auto pathTexture = mr.Read<alflib::String>();
    return asRenderable(String{pathTexture.GetUTF8()});
  }
  case ComponentType::kRotor: {
    Quat q{};
    q.x = mr.Read<decltype(q.x)>();
    q.y = mr.Read<decltype(q.y)>();
    q.z = mr.Read<decltype(q.z)>();
    q.w = mr.Read<decltype(q.w)>();
    return asRotor(q);
  }
  case ComponentType::kCollider: {
    f32 restitution = mr.Read<f32>();
    f32 mass = mr.Read<f32>();
    return asCollider(restitution, mass);
  }
  case ComponentType::kWindAffectable: {
    return asWindAffectable();
  }
  default: {
    Util::panic("Invalid ComponentData variant: {}", type);
  }
  }
}

// -------------------------------------------------------------------------- //

ComponentData ComponentData::asRigidbody() {
  ComponentData data;
  data.m_data = RigidbodyData{};
  return data;
}

// -------------------------------------------------------------------------- //

ComponentData ComponentData::asWind(const std::vector<BaseFn> &functions,
                                    Vec3F offset, u8 volumeType) {
  ComponentData data;
  data.m_data = WindData{functions, offset, volumeType};
  return data;
}

// -------------------------------------------------------------------------- //

ComponentData ComponentData::asRenderable(const String &pathTexture) {
  ComponentData data;
  data.m_data = RenderableData{pathTexture};
  return data;
}

// -------------------------------------------------------------------------- //

ComponentData ComponentData::asRotor(const Quat &q) {
  ComponentData data;
  data.m_data = RotorData{q};
  return data;
}

// -------------------------------------------------------------------------- //

ComponentData ComponentData::asCollider(f32 restitution, f32 mass) {
  ComponentData data;
  data.m_data = ColliderData{restitution, mass};
  return data;
}

ComponentData ComponentData::asWindAffectable() {
  ComponentData data;
  data.m_data = WindAffectableData{};
  return data;
}

} // namespace wind
