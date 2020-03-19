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

#include "csim.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/scene/builder.hpp"

#include "microprofile/microprofile.h"

// ========================================================================== //
// CSpline Implementation
// ========================================================================== //

namespace wind {

CSim::CSim(const bs::HSceneObject &parent) : CPaint(parent) {}

// -------------------------------------------------------------------------- //

void CSim::build(s32 width, s32 height, s32 depth, f32 cellSize,
                 const bs::SPtr<bs::SceneInstance> &scene) {
  bs::SPtr<bs::SceneInstance> _scene = scene;
  if (!scene) {
    _scene = bs::SceneManager::instance().getMainScene();
  }

  m_sim = new WindSimulation(width, height, depth, cellSize);
  m_sim->buildForScene(_scene);
}

// -------------------------------------------------------------------------- //

bs::HSceneObject CSim::bake() {
  return ObjectBuilder(ObjectType::kEmpty).build();
}

// -------------------------------------------------------------------------- //

void CSim::paint(Painter &painter) { m_sim->paint(painter); }

// -------------------------------------------------------------------------- //

void CSim::fixedUpdate() {
  const f32 delta = bs::gTime().getFixedFrameDelta();
  m_sim->step(delta);
}

// -------------------------------------------------------------------------- //

bs::RTTITypeBase *CSim::getRTTIStatic() { return CSimRTTI::instance(); }

} // namespace wind
