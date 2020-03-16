// MIT License
//
// Copyright (c) 2019-2020 Filip Björklund, Christoffer Gustafsson
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

#include "debug_manager.hpp"

// ========================================================================== //
// DebugManager Implementation
// ========================================================================== //

namespace wind {

s32 DebugManager::getS32(const String &name) {
  return instance().m_mapS32[name];
}

// -------------------------------------------------------------------------- //

void DebugManager::setS32(const String &name, s32 value) {
  instance().m_mapS32[name] = value;
}

// -------------------------------------------------------------------------- //

f32 DebugManager::getF32(const String &name) {
  return instance().m_mapF32[name];
}

// -------------------------------------------------------------------------- //

void DebugManager::setF32(const String &name, f32 value) {
  instance().m_mapF32[name] = value;
}
// -------------------------------------------------------------------------- //

bool DebugManager::getBool(const String &name) {
  return instance().m_mapBool[name];
}

// -------------------------------------------------------------------------- //

void DebugManager::setBool(const String &name, bool value) {
  instance().m_mapBool[name] = value;
}

// -------------------------------------------------------------------------- //

String DebugManager::getString(const String &name) {
  return instance().m_mapString[name];
}

// -------------------------------------------------------------------------- //

void DebugManager::setString(const String &name, const String &value) {
  instance().m_mapString[name] = value;
}

// -------------------------------------------------------------------------- //

DebugManager &DebugManager::instance() {
  static DebugManager manager;
  return manager;
}

} // namespace wind
