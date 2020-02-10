// MIT License
//
// Copyright (c) 2019 Filip Björklund
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

#include "alflib/collection/array_map.hpp"
#include "alflib/graphics/image.hpp"

// ========================================================================== //
// ImageAtlas Declaration
// ========================================================================== //

namespace alflib {

/** \struct ImageAtlasRegion
 * \author Filip Björklund
 * \date 24 june 2019 - 11:22
 * \brief Image atlas region.
 * \details
 * Structure that represents a region on an ImageAtlas. A region is information
 * about a sub-image of the atlas.
 */
struct ImageAtlasRegion
{
  /** X position of the region on the atlas **/
  u32 x;
  /** Y position of the region on the atlas **/
  u32 y;
  /** Width of the region on the atlas **/
  u32 width;
  /** Height of the region on the atlas **/
  u32 height;
};

// -------------------------------------------------------------------------- //

/** \class ImageAtlas
 * \author Filip Björklund
 * \date 24 june 2019 - 11:25
 * \brief Image atlas.
 * \details
 * Class that represents an image atlas where a set of smaller images are all
 * copied onto. Each image that is placed on the atlas can be retrieved using an
 * unique name.
 */
template<typename MapType = ArrayMap<String, ImageAtlasRegion>>
class ImageAtlas
{
public:
  /** Enumeration of results **/
  enum class Result
  {
    /** Success **/
    kSuccess,
    /** Atlas cannot fit all images **/
    kAtlasTooSmall
  };

  /** Enumeration of packing strategies **/
  enum class PackingStrategy
  {
    /** Pack in linear order. From first to last **/
    kLinear,
    /** Pack in order of ascending height **/
    kHeightSorted
  };

private:
  /** Atlas image. All images are blitted onto this during creation **/
  Image mAtlas;
  /** Map of regions in atlas **/
  MapType mRegionMap;

public:
  /** Construct an image atlas without actually building the underlying atlas.
   * The function 'ImageAtlas::Build' must be called to build the atlas.
   * \brief Construct image atlas.
   */
  ImageAtlas() = default;

  /** Build the atlas from a set of images and their corresponding identifying
   * names. Each image is packed onto a larger image (atlas) and their locations
   * are recorded in a map with the names used for lookup.
   * \brief Build atlas from images.
   * \param images List of images to pack into atlas.
   * \param names Names of images.
   * \param width Width of the atlas. This, together with the height must be
   * large enough to be able to hold the entire set of images.
   * \param height Height of the atlas. This, together with the width must be
   * large enough to be able to hold the entire set of images.
   * \param packingStrategy Strategy used for packing the images.
   */
  Result Build(const ArrayList<Image*>& images,
               const ArrayList<String>& names,
               u32 width,
               u32 height,
               PackingStrategy packingStrategy = PackingStrategy::kLinear);

  /** Build the atlas from a set of images and their corresponding identifying
   * names. Each image is packed onto a larger image (atlas) and their locations
   * are recorded in a map with the names used for lookup.
   * \brief Build atlas from images.
   * \param images List of images to pack into atlas.
   * \param names Names of images.
   * \param width Width of the atlas. This, together with the height must be
   * large enough to be able to hold the entire set of images.
   * \param height Height of the atlas. This, together with the width must be
   * large enough to be able to hold the entire set of images.
   * \param packingStrategy Strategy used for packing the images.
   */
  Result Build(const ArrayList<Image>& images,
               const ArrayList<String>& names,
               u32 width,
               u32 height,
               PackingStrategy packingStrategy = PackingStrategy::kLinear);

  /** Returns the atlas image that is the image that all sub-images specified
   * during construction are placed upon.
   * \brief Returns atlas image.
   * \return Atlas image.
   */
  Image& GetImage() { return mAtlas; }

  /** Returns the atlas image that is the image that all sub-images specified
   * during construction are placed upon.
   * \brief Returns atlas image.
   * \return Atlas image.
   */
  const Image& GetImage() const { return mAtlas; }

  /** Returns whether or not the image atlas has a region with the specified
   * name as key.
   * \brief Returns whether region with specified name is part of atlas.
   * \param name Name to lookup.
   * \return True if a region with the name exists otherwise false.
   */
  bool HasRegion(const String& name) const { return mRegionMap.HasKey(name); }

  /** Returns the region of the atlas that is identified by the specified name.
   * \pre Name must identify a valid region in the atlas. Use
   * ImageAtlas::HasRegion() to verify that the region is valid.
   * \brief Returns region identified by name.
   * \param name Name of the region to retrieve.
   * \return Region.
   */
  const ImageAtlasRegion& GetRegion(const String& name) const
  {
    return mRegionMap.At(name);
  }

  /** Returns the width of the atlas in pixels.
   * \brief Returns width.
   * \return Width of the atlas.
   */
  u32 GetWidth() const { return mAtlas.GetWidth(); }

  /** Returns the height of the atlas in pixels.
   * \brief Returns height.
   * \return Height of the atlas.
   */
  u32 GetHeight() const { return mAtlas.GetHeight(); }
};

// -------------------------------------------------------------------------- //

template<typename MapType>
typename ImageAtlas<MapType>::Result
ImageAtlas<MapType>::Build(const ArrayList<Image*>& images,
                           const ArrayList<String>& names,
                           u32 width,
                           u32 height,
                           PackingStrategy packingStrategy)
{
  // Only linear packing supported
  AlfAssert(packingStrategy == PackingStrategy::kLinear,
            "Only linear packing supported");

  // Create atlas
  mAtlas.Create(width, height);
  mAtlas.Fill(Color::WHITE);

  // Blit images
  u32 x = 0, y = 0, maxHeight = 0;
  for (u32 i = 0; i < images.GetSize(); i++) {
    Image* image = images[i];

    // Begin another row?
    if (x + image->GetWidth() > width) {
      x = 0;
      y += maxHeight;
      maxHeight = 0;
    }

    // Does not fit?
    if (y + image->GetHeight() > height) {
      return Result::kAtlasTooSmall;
    }

    // Blit image
    mAtlas.Blit(*image, x, y);
    mRegionMap[names[i]] =
      ImageAtlasRegion{ x, y, image->GetWidth(), image->GetHeight() };
    x += image->GetWidth();

    // Increase max height of current row?
    if (image->GetHeight() > maxHeight) {
      maxHeight = image->GetHeight();
    }
  }

  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

template<typename MapType>
typename ImageAtlas<MapType>::Result
ImageAtlas<MapType>::Build(const ArrayList<Image>& images,
                           const ArrayList<String>& names,
                           u32 width,
                           u32 height,
                           PackingStrategy packingStrategy)
{
  // Only linear packing supported
  AlfAssert(packingStrategy == PackingStrategy::kLinear,
            "Only linear packing supported");

  // Create atlas
  mAtlas.Create(width, height);
  mAtlas.Fill(Color::WHITE);

  // Blit images
  u32 x = 0, y = 0, maxHeight = 0;
  for (u32 i = 0; i < images.GetSize(); i++) {
    const Image& image = images[i];

    // Begin another row?
    if (x + image.GetWidth() > width) {
      x = 0;
      y += maxHeight;
      maxHeight = 0;
    }

    // Does not fit?
    if (y + image.GetHeight() > height) {
      return Result::kAtlasTooSmall;
    }

    // Blit image
    mAtlas.Blit(image, x, y);
    mRegionMap[names[i]] =
      ImageAtlasRegion{ x, y, image.GetWidth(), image.GetHeight() };
    x += image.GetWidth();

    // Increase max height of current row?
    if (image.GetHeight() > maxHeight) {
      maxHeight = image.GetHeight();
    }
  }

  return Result::kSuccess;
}

}