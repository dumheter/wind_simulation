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

#include <tuple>

#include "common.hpp"

#include "BsCorePrerequisites.h"
#include "Resources/BsResource.h"

// ========================================================================== //
// Asset Declaration
// ========================================================================== //

namespace wind {

/* Asset utilities */
class Asset {
private:
  Asset() = delete;
  ~Asset() = delete;

public:
  /* Load texture from file. An attempt will first be made to load an '.asset'
   * file for the path, otherwise the resource will be imported */
  static bs::HTexture loadTexture(const bs::Path &path, bool srgb = true,
                                  bool hdr = false, bool mipmaps = true);

  /* Load cubemap from file. An attempt will first be made to load an '.asset'
   * file for the path, otherwise the resource will be imported */
  static bs::HTexture loadCubemap(const bs::Path &path, bool srgb = true,
                                  bool hdr = false);

  /* Load mesh from file. An attempt will first be made to load an '.asset'
   * file for the path, otherwise the resource will be imported */
  static bs::HMesh loadMesh(const bs::Path &path, f32 scale = 1.0f,
                            bool cpuCached = false);

  /* Load mesh from file. An attempt will first be made to load an '.asset'
   * file for the path, otherwise the resource will be imported */
  static std::tuple<bs::HMesh, bs::HPhysicsMesh>
  loadMeshWithPhysics(const bs::Path &path, f32 scale = 1.0f,
                      bool cpuCached = false);
};

} // namespace wind