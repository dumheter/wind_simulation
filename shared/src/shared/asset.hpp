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
#include <variant>

#include "types.hpp"

#include <BsCorePrerequisites.h>

// ========================================================================== //
// Asset Declaration
// ========================================================================== //

namespace wind {

/* Asset utilities */
class Asset {
public:
  /// Texture asset
  struct Texture {
    bs::HTexture handle;
  };

  /// Mesh asset
  struct Mesh {
    bs::HMesh handle;
  };

  // Handle type
  using Handle = std::variant<Texture, Mesh>;

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
                      bool cpuCached = false, bool isConvex = false);

  /// Returns whether or not a handle is of a specific type
  template <typename T> static bool isType(const Handle &handle) {
    return std::holds_alternative<T>(handle);
  }

  /// Returns the texture handle from an asset handle
  static bs::HTexture handleTexture(const Handle &handle) {
    return std::get<Texture>(handle).handle;
  }
};

} // namespace wind

// ========================================================================== //
// AssetManager Declaration
// ========================================================================== //

namespace wind {

///
class AssetManager {
public:
  /// Load a texture
  static bs::HTexture loadTexture(const bs::Path &path, bool srgb = true,
                                  bool hdr = false);

  /// Returns the path for a texture asset (inefficient)
  static bool getTexturePath(const bs::HTexture &texture, bs::Path &pathOut);

private:
  ///
  AssetManager() = default;

  ///
  ~AssetManager() = default;

  ///
  static AssetManager &instance();

  ///
  std::unordered_map<bs::Path, Asset::Handle> m_assetMap;
};

} // namespace wind
