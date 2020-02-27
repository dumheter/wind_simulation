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

#include "component_data.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/log.hpp"
#include "shared/utility/util.hpp"

// ========================================================================== //
// ComponentData Implementation
// ========================================================================== //

namespace wind {

const ComponentData::WindSourceData &ComponentData::windSourceData() const {
  return std::get<WindSourceData>(m_data);
}

// -------------------------------------------------------------------------- //

const ComponentData::RigidbodyData &ComponentData::rigidbodyData() const {
  return std::get<RigidbodyData>(m_data);
}

const ComponentData::RenderableData &ComponentData::renderableData() const {
  return std::get<RenderableData>(m_data);
}

// -------------------------------------------------------------------------- //

bool ComponentData::ToBytes(alflib::RawMemoryWriter &mw) const {
  if (std::holds_alternative<WindSourceData>(m_data)) {
    mw.Write(static_cast<TagType>(ComponentType::kWindSource));
    return mw.Write(std::get<WindSourceData>(m_data).functions);
  } else if (std::holds_alternative<RigidbodyData>(m_data)) {
    mw.Write(static_cast<TagType>(ComponentType::kRigidbody));
    mw.Write(std::get<RigidbodyData>(m_data).restitution);
    return mw.Write(std::get<RigidbodyData>(m_data).mass);
  } else if (std::holds_alternative<RenderableData>(m_data)) {
    mw.Write(static_cast<TagType>(ComponentType::kRenderable));
    return mw.Write(
        alflib::String(std::get<RenderableData>(m_data).pathTexture.c_str()));
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
    f32 restitution = mr.Read<f32>();
    f32 mass = mr.Read<f32>();
    return asRigidbody(restitution, mass);
  }
  case ComponentType::kWindSource: {
    std::vector<BaseFn> functions = mr.ReadStdVector<BaseFn>();
    return asWindSource(functions);
  }
  case ComponentType::kRenderable: {
    auto pathTexture = mr.Read<alflib::String>();
    return asRenderable(String{pathTexture.GetUTF8()});
  }
  case ComponentType::kRotor:
  default: {
    Util::panic("Invalid ComponentData variant");
  }
  }
}

// -------------------------------------------------------------------------- //

ComponentData
ComponentData::asWindSource(const std::vector<BaseFn> &functions) {
  ComponentData data;
  data.m_data = WindSourceData{functions};
  return data;
}

// -------------------------------------------------------------------------- //

ComponentData ComponentData::asRigidbody(f32 restitution, f32 mass) {
  ComponentData data;
  data.m_data = RigidbodyData{restitution, mass};
  return data;
}

ComponentData ComponentData::asRenderable(const String& pathTexture) {
  ComponentData data;
  data.m_data = RenderableData{pathTexture};
  return data;
}

} // namespace wind
