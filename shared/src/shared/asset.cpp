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

#include "asset.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/log.hpp"
#include "shared/utility/util.hpp"

#include <Image/BsTexture.h>
#include <Importer/BsImporter.h>
#include <Importer/BsMeshImportOptions.h>
#include <Importer/BsShaderImportOptions.h>
#include <Importer/BsTextureImportOptions.h>
#include <Reflection/BsRTTIType.h>
#include <Resources/BsResourceHandle.h>
#include <Resources/BsResourceManifest.h>
#include <Resources/BsResources.h>

// ========================================================================== //
// Asset Implementation
// ========================================================================== //

namespace wind {

bs::HTexture Asset::loadTexture(const bs::Path &path, bool srgb, bool hdr,
                                bool mipmaps) {
  if (!Util::fileExist(path)) {
    return nullptr;
  }

  bs::Path assetPath = path;
  assetPath.setExtension(path.getExtension() + ".asset");

  bs::HTexture texture = bs::gResources().load<bs::Texture>(assetPath);
  if (!texture) {
    bs::gDebug().log(
        "Texture '" + path.toString() +
            "' has not yet been imported. This process can take a while",
        bs::LogVerbosity::Warning);

    const bs::SPtr<bs::ImportOptions> _impOpt =
        bs::Importer::instance().createImportOptions(path);
    if (bs::rtti_is_of_type<bs::TextureImportOptions>(_impOpt)) {
      bs::TextureImportOptions *impOpt =
          static_cast<bs::TextureImportOptions *>(_impOpt.get());
      impOpt->sRGB = srgb;
      if (hdr) {
        impOpt->format = bs::PF_RG11B10F;
      }
      impOpt->generateMips = mipmaps;
      impOpt->cpuCached = true;
      impOpt->cubemap = false;
    }
    texture = bs::gImporter().import<bs::Texture>(path, _impOpt);
    bs::gResources().save(texture, assetPath, true);
  }

  bs::gResources().getResourceManifest("Default")->registerResource(
      texture.getUUID(), assetPath);

  return texture;
}

// -------------------------------------------------------------------------- //

bs::HTexture Asset::loadCubemap(const bs::Path &path, bool srgb, bool hdr) {
  if (!Util::fileExist(path)) {
    return nullptr;
  }

  bs::Path assetPath = path;
  assetPath.setExtension(path.getExtension() + ".asset");

  bs::HTexture texture = bs::gResources().load<bs::Texture>(assetPath);
  if (!texture) {
    bs::gDebug().log(
        "Cubemap texture '" + path.toString() +
            "' has not yet been imported. This process can take a while",
        bs::LogVerbosity::Warning);

    const bs::SPtr<bs::ImportOptions> _impOpt =
        bs::Importer::instance().createImportOptions(path);
    if (bs::rtti_is_of_type<bs::TextureImportOptions>(_impOpt)) {
      bs::TextureImportOptions *impOpt =
          static_cast<bs::TextureImportOptions *>(_impOpt.get());
      impOpt->sRGB = srgb;
      if (hdr) {
        impOpt->format = bs::PF_RG11B10F;
      }
      impOpt->generateMips = true;
      impOpt->cpuCached = true;
      impOpt->cubemap = true;
      impOpt->cubemapSourceType = bs::CubemapSourceType::Cylindrical;
    }
    texture = bs::gImporter().import<bs::Texture>(path, _impOpt);
    bs::gResources().save(texture, assetPath, true);
  }

  bs::gResources().getResourceManifest("Default")->registerResource(
      texture.getUUID(), assetPath);

  return texture;
}

// -------------------------------------------------------------------------- //

bs::HMesh Asset::loadMesh(const bs::Path &path, f32 scale, bool cpuCached) {
  if (!Util::fileExist(path)) {
    return nullptr;
  }

  bs::Path assetPath = path;
  assetPath.setExtension(path.getExtension() + ".asset");

  bs::HMesh mesh = bs::gResources().load<bs::Mesh>(assetPath);
  if (!mesh) {
    bs::gDebug().log(
        "Mesh '" + path.toString() +
            "' has not yet been imported. This process can take a while",
        bs::LogVerbosity::Warning);

    const bs::SPtr<bs::ImportOptions> _impOpt =
        bs::Importer::instance().createImportOptions(path);
    if (bs::rtti_is_of_type<bs::MeshImportOptions>(_impOpt)) {
      bs::MeshImportOptions *impOpt =
          static_cast<bs::MeshImportOptions *>(_impOpt.get());
      impOpt->cpuCached = cpuCached;
      impOpt->importNormals = true;
      impOpt->importTangents = true;
      impOpt->importScale = scale;
    }
    mesh = bs::gImporter().import<bs::Mesh>(path, _impOpt);
    bs::gResources().save(mesh, assetPath, true);
  }

  bs::gResources().getResourceManifest("Default")->registerResource(
      mesh.getUUID(), assetPath);

  return mesh;
}

// -------------------------------------------------------------------------- //

std::tuple<bs::HMesh, bs::HPhysicsMesh>
Asset::loadMeshWithPhysics(const bs::Path &path, f32 scale, bool cpuCached,
                           bool isConvex) {
  if (!Util::fileExist(path)) {
    return std::make_tuple(nullptr, nullptr);
  }

  bs::Path assetPath = path;
  assetPath.setExtension(path.getExtension() + ".asset");
  bs::Path physAssetPath = path;
  physAssetPath.setExtension(path.getExtension() + ".phys.asset");

  // Try to load assets
  bs::HMesh mesh = bs::gResources().load<bs::Mesh>(assetPath);
  bs::HPhysicsMesh physMesh =
      bs::gResources().load<bs::PhysicsMesh>(physAssetPath);

  // Otherwise import them
  if (!mesh) {
    bs::gDebug().log(
        "Mesh '" + path.toString() +
            "' has not yet been imported. This process can take a while",
        bs::LogVerbosity::Warning);

    const bs::SPtr<bs::ImportOptions> _impOpt =
        bs::Importer::instance().createImportOptions(path);
    if (bs::rtti_is_of_type<bs::MeshImportOptions>(_impOpt)) {
      auto meshType = isConvex ? bs::CollisionMeshType::Convex
                               : bs::CollisionMeshType::Normal;
      bs::MeshImportOptions *impOpt =
          static_cast<bs::MeshImportOptions *>(_impOpt.get());
      impOpt->cpuCached = cpuCached;
      impOpt->importNormals = true;
      impOpt->importTangents = true;
      impOpt->importScale = scale;
      impOpt->collisionMeshType = meshType;
    }

    // Import mesh and physics mesh
    auto res = bs::gImporter().importAll(path, _impOpt);
    mesh = bs::static_resource_cast<bs::Mesh>(res->entries[0].value);
    physMesh = bs::static_resource_cast<bs::PhysicsMesh>(res->entries[1].value);

    bs::gResources().save(mesh, assetPath, true);
    bs::gResources().save(physMesh, physAssetPath, true);
  }

  bs::gResources().getResourceManifest("Default")->registerResource(
      mesh.getUUID(), assetPath);
  // gResources().getResourceManifest("Default")->registerResource(
  //    physMesh.getUUID(), assetPath);

  return std::make_tuple(mesh, physMesh);
}

// -------------------------------------------------------------------------- //

bs::HShader Asset::loadShader(const bs::Path &path) {
  if (!Util::fileExist(path)) {
    return nullptr;
  }

  bs::Path assetPath = path;
  assetPath.setExtension(path.getExtension() + ".asset");
  bs::HShader shader = bs::gResources().load<bs::Shader>(assetPath);

  if (!shader) {
    bs::gDebug().log(
        "Shader '" + path.toString() +
            "' has not yet been imported. This process can take a while",
        bs::LogVerbosity::Warning);

    const bs::SPtr<bs::ImportOptions> _impOpt =
        bs::Importer::instance().createImportOptions(path);
    if (bs::rtti_is_of_type<bs::ShaderImportOptions>(_impOpt)) {
      const bs::ShaderImportOptions *impOpt =
          static_cast<bs::ShaderImportOptions *>(_impOpt.get());
    }
    shader = bs::gImporter().import<bs::Shader>(path, _impOpt);
    bs::gResources().save(shader, assetPath, true);
  }

  bs::gResources().getResourceManifest("Default")->registerResource(
      shader.getUUID(), assetPath);

  return shader;
}

} // namespace wind

// ========================================================================== //
// AssetManager Implementation
// ========================================================================== //

namespace wind {

bs::HTexture AssetManager::loadTexture(const bs::Path &path, bool srgb,
                                       bool hdr) {
  AssetManager &mgr = instance();
  if (mgr.m_assetMap.count(path) > 0) {
    Asset::Handle handle = mgr.m_assetMap[path];
    return std::get<Asset::Texture>(handle).handle;
  } else {
    bs::HTexture textureHandle = Asset::loadTexture(path, srgb, hdr);
    Asset::Handle handle = Asset::Texture{textureHandle};
    mgr.m_assetMap[path] = handle;
    return textureHandle;
  }
}

// -------------------------------------------------------------------------- //

bool AssetManager::getTexturePath(const bs::HTexture &texture,
                                  bs::Path &pathOut) {

  AssetManager &mgr = instance();
  for (const auto &entry : mgr.m_assetMap) {
    const bs::Path &path = entry.first;
    const Asset::Handle &handle = entry.second;
    if (Asset::isType<Asset::Texture>(handle)) {
      bs::HTexture _texture = Asset::handleTexture(handle);
      if (_texture == texture) {
        pathOut = path;
        return true;
      }
    }
  }
  return false;
}

// -------------------------------------------------------------------------- //

AssetManager &AssetManager::instance() {
  static AssetManager manager;
  return manager;
}

} // namespace wind
