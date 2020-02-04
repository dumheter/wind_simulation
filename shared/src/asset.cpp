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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
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

#include <Resources/BsResources.h>
#include <Importer/BsImporter.h>
#include <Importer/BsTextureImportOptions.h>
#include <Reflection/BsRTTIType.h>

// ========================================================================== //
// Asset Implementation
// ========================================================================== //

namespace wind
{

bs::HTexture Asset::loadTexture(const bs::Path &path, bool srgb, bool hdr, bool mipmaps)
{
    using namespace bs;

    Path assetPath = path;
    assetPath.setExtension(path.getExtension() + ".asset");

    HTexture texture = gResources().load<Texture>(assetPath);
    if (!texture)
    {
        gDebug().logWarning("Texture '" + path.toString() +
                            "' has not yet been imported. This process can take a while");

        const SPtr<ImportOptions> _impOpt = Importer::instance().createImportOptions(path);
        if (rtti_is_of_type<TextureImportOptions>(_impOpt))
        {
            TextureImportOptions *impOpt = static_cast<TextureImportOptions *>(_impOpt.get());
            impOpt->setSRGB(srgb);
            if (hdr)
            {
                impOpt->setFormat(PF_RG11B10F);
            }
            impOpt->setGenerateMipmaps(mipmaps);
            impOpt->setCPUCached(true);
            impOpt->setIsCubemap(false);
        }
        texture = gImporter().import<Texture>(path, _impOpt);
        gResources().save(texture, assetPath, true);
    }
    return texture;
}

// -------------------------------------------------------------------------- //

bs::HTexture Asset::loadCubemap(const bs::Path &path, bool srgb, bool hdr)
{
    using namespace bs;

    Path assetPath = path;
    assetPath.setExtension(path.getExtension() + ".asset");

    HTexture texture = gResources().load<Texture>(assetPath);
    if (!texture)
    {
        gDebug().logWarning("Cubemap texture '" + path.toString() +
                            "' has not yet been imported. This process can take a while");

        const SPtr<ImportOptions> _impOpt = Importer::instance().createImportOptions(path);
        if (rtti_is_of_type<TextureImportOptions>(_impOpt))
        {
            TextureImportOptions *impOpt = static_cast<TextureImportOptions *>(_impOpt.get());
            impOpt->setSRGB(srgb);
            if (hdr)
            {
                impOpt->setFormat(PF_RG11B10F);
            }
            impOpt->setGenerateMipmaps(true);
            impOpt->setCPUCached(true);
            impOpt->setIsCubemap(true);
            impOpt->setCubemapSourceType(CubemapSourceType::Cylindrical);
        }
        texture = gImporter().import<Texture>(path, _impOpt);
        gResources().save(texture, assetPath, true);
    }
    return texture;
}

} // namespace wind
