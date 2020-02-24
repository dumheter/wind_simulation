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

#include "util.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "log.hpp"

#include <BsApplication.h>
#include <Input/BsMouse.h>
#include <Platform/BsCursor.h>

// ========================================================================== //
// Functions
// ========================================================================== //

namespace wind {

void dumpSceneAux(const bs::HSceneObject &o, u32 indent) {
  std::string pad = Util::repeat(' ', indent);
  const char *name = o->getName().c_str();
  logInfo("{}{}", pad, name);
  for (u32 i = 0; i < o->getNumChildren(); ++i) {
    dumpSceneAux(o->getChild(i), indent + 2);
  }
}

} // namespace wind

// ========================================================================== //
// Util Implementation
// ========================================================================== //

namespace wind {

void Util::centerCursor(bs::SPtr<bs::RenderWindow> window) {
  using namespace bs;

  if (!window) {
    window = gApplication().getPrimaryWindow();
  }
  const RenderWindowProperties &windowProps = window->getProperties();
  const UINT32 x = windowProps.left + (windowProps.width / 2);
  const UINT32 y = windowProps.top + (windowProps.height / 2);
  Cursor::instance().setScreenPosition(Vector2I(x, y));
}

// -------------------------------------------------------------------------- //

void Util::dumpScene(const bs::HSceneObject &scene) { dumpSceneAux(scene, 0); }

// -------------------------------------------------------------------------- //

std::string Util::repeat(char c, u32 n) {
  std::string str;
  str.resize(n);
  for (u32 i = 0; i < n; i++) {
    str[i] = c;
  }
  return str;
}

} // namespace wind
