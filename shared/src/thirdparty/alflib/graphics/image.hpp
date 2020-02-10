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

// Project headers
#include "alflib/file/path.hpp"
#include "alflib/graphics/color.hpp"

// ========================================================================== //
// Image Declaration
// ========================================================================== //

namespace alflib {

/** \class Image
 * \author Filip Björklund
 * \date 22 june 2019 - 15:34
 * \brief Image.
 * \details
 * Class that represents an image.
 */
class Image
{
public:
  /** Results enumeration **/
  enum class Result
  {
    /** Success **/
    kSuccess,
    /** Failed allocation **/
    kOutOfMemory,
    /** Image file could not be found **/
    kFileNotFound,
    /** Invalid file format for an image **/
    kInvalidFileFormat,
    /** Failed to write image file **/
    kFailedToWriteFile,
    /** Image file already exists **/
    kImageFileAlreadyExists
  };

  /** Enumeration of image formats  **/
  enum class Format
  {
    /** Unknown format. It's only used as an argument to functions, images
       cannot exist in this format **/
    kUnknown,
    /** Grayscale format **/
    kGrayscale,
    /** RGBA format **/
    kRGBA,
    /** RGB format **/
    kRGB,
    /** BGRA format **/
    kBGRA,
    /** BGR format **/
    kBGR
  };

  /** Enumeration of image resize filters **/
  enum class Filter
  {
    /** Default filter **/
    kDefault = 0,
    /** Box filter **/
    kBox = kDefault,
    /** Triangle filter **/
    kTriangle,
    /** Cubic spline filter **/
    kCubicSpline,
    /** Catmull rom filter **/
    kCatmullRom,
    /** Mitchell filter **/
    kMitchell
  };

private:
  /** Width of the image in pixels **/
  u32 mWidth = 0;
  /** Height of the image in pixels **/
  u32 mHeight = 0;
  /** Format of the image **/
  Format mFormat = Format::kUnknown;

  /** Allocator **/
  Allocator& mAllocator;
  /** Image data **/
  u8* mData = nullptr;
  /** Size of image data in bytes **/
  u64 mDataSize = 0;

public:
  /** Construct an empty image without dimensions and data. The image data must
   * either be setup by a call to Image::Load(...), to load from a file, or by a
   * call to Image::Create(...), to create the image from data.
   * \param allocator Allocator to allocate data with.
   */
  explicit Image(Allocator& allocator = DefaultAllocator::Instance());

  /** Copy-constructor **/
  Image(const Image& other);

  /** Move-constructor **/
  Image(Image&& other);

  /** Destruct image and free data **/
  ~Image();

  /** Copy-assignment **/
  Image& operator=(const Image& other);

  /** Move-assignment **/
  Image& operator=(Image&& other);

  /** Load the image data from a file at the specified path. An optional format
   * can be specified that the data should be converted to after loading it from
   * the file. If the format is Format::kUnknown (default) then the format is
   * determined from the file.
   * \param path Path to the image file.
   * \param format Format to store the image in. If the format Format::kUnknown
   * is specified then the image is stored in the format determined by the file.
   * \return Result.
   * Result::kSuccess: Successfully loaded image data from file.
   * Result::kFileNotFound: Failed to find the image file.
   * Result::kOutOfMemory: Failed to allocate memory for the image data.
   */
  Result Load(const Path& path, Format format = Format::kUnknown);

  /** Create the image data from the specified dimensions and format. Data for
   * the image can also be specified, along with a format for the data.
   * \note The data is copied by the image if it's not null, therefore the data
   * is still owned by the caller.
   * \note If the data format is Format::kUnknown, then the data is assumed to
   * have the same format as the constructed image.
   * \note The data is expected to be at least the size that is required by the
   * image (width * height * format-bytes-per-pixel). Any size less than that
   * gives undefined behaviour.
   * \pre Width must be greater than zero.
   * \pre Height must be greater than zero.
   * \pre Format cannot be unknown.
   * \brief Create image data.
   * \param width Width of the image.
   * \param height Height of the image.
   * \param format Format of the image.
   * \param data Data to initialize image from. If the data format is not
   * Format::kUnknown and the format does no match the image format then the
   * data is converted to the correct format before being used to initialize the
   * image.
   * \param dataFormat Format of the data.
   * \return Result.
   * Result::kSuccess: Successfully loaded image data from file.
   * Result::kOutOfMemory: Failed to allocate memory for the image data.
   */
  Result Create(u32 width,
                u32 height,
                Format format = Format::kRGBA,
                const u8* data = nullptr,
                Format dataFormat = Format::kUnknown);

  /** Returns a copy of the image converted to the specified target format.
   * \brief Returns conversion of image.
   * \param[in] targetFormat Format to returns a conversion of the image in.
   * \param[out] imageOut Converted image is stored in this parameter.
   * \return Result.
   * - Result::kSuccess: Succesfully converted image.
   */
  Result ConvertedTo(Format targetFormat, Image& image);

  /** Returns a resize copy of the image. The resized image is written to the
   * 'image' output parameter.
   * \brief Returns resized copy.
   * \param[in] width Width to resize to.
   * \param[in] height Height to resize to.
   * \param[in] filter Filter to use for resize operations.
   * \param[out] image Image to store result in.
   * \return Result.
   * - Result::kSuccess: Successfully resized image.
   */
  Result ResizedTo(u32 width, u32 height, Filter filter, Image& image);

  /** Blit parts of (or whole of) another image onto this image. This means
   * copying pixels from the source image onto this (destination) image.
   *
   * Source offset specifies where in the source image that the
   * operation should begin copying data. While the destination offsets specify
   * where in the destination image (this image) that the operation should begin
   * copying data to.
   *
   * Width and height determine how much data should be copied
   * from the source image onto the destination image.
   *
   * \note If the formats of the two images match then the operation can be
   * greatly sped up due to not having to convert the pixel data (memory can be
   * directly copied).
   *
   * \note If not values are specified for the width and height then the default
   * values (U32_MAX) has the special meaning of copying the entire source
   * image.
   *
   * \pre The width plus either of the two X offsets must not be outside of the
   * width bounds of the respective texture.
   * \pre The height plus either of the two Y offsets must not be outside of the
   * height bounds of the respective texture.
   *
   * \brief Blit image onto this image.
   * \param srcImage Image to blit onto this image.
   * \param dstOffsetX X position in the destination image to begin copying
   * data to
   * \param dstOffsetY Y position in the destination image to begin copying
   * data to
   * \param srcOffsetX X position in source image to begin copying data at.
   * \param srcOffsetY Y position in source image to begin copying data at.
   * \param width Width of the copied region.
   * \param height Height of the copied region.
   * \return Result.
   * - Result::kSuccess: successfully copied (blitted) data.
   */
  Result Blit(const Image& srcImage,
              u32 dstOffsetX,
              u32 dstOffsetY,
              u32 srcOffsetX = 0,
              u32 srcOffsetY = 0,
              u32 width = U32_MAX,
              u32 height = U32_MAX);

  /** Fill the image with a specified color.
   * \brief Fill image.
   * \param color Color to fill image with.
   */
  void Fill(const Color& color);

  /** Save the image to a file specified by the 'path'. It can also be specified
   * whether to overwrite any existing file at the same path.
   * \note The extension of the 'path' must be one of the ones supported:
   * - Path::Extension::kPng,
   * - Path::Extension::kTga,
   * - Path::Extension::kJpeg,
   * \brief Save image to file.
   * \param path Path to save image at.
   * \param overwrite Whether to overwrite any existing file.
   * \return Result.
   * Result::kSuccess: Successfully saved file.
   */
  Result Save(const Path& path, bool overwrite = false);

  /** Returns the color of the pixel at the specified 'x' and 'y' position in
   * the image, specified from the top left.
   * \pre Image must be in a valid format.
   * \brief Returns color of pixel.
   * \param x X position of pixel
   * \param y Y position of pixel.
   * \return Color of pixel at the specified location.
   */
  Color GetPixel(u32 x, u32 y) const;

  /** Set the color of a pixel at the specified 'x' and 'y' position in the
   * image, specified from the top left.
   * \pre Image must be in a valid format.
   * \brief Set color of pixel.
   * \param x X position of pixel
   * \param y Y position of pixel.
   * \param color Color to set pixel to.
   */
  void SetPixel(u32 x, u32 y, const Color& color);

  /** Apply a function to all of the pixels in the image. The function should
   * have the prototype '(Image& image, u32 x, u32 y) -> (Color)'. Any number of
   * arguments can also be specified that is forwarded to the function.
   * \brief Apply function to all pixels.
   * \tparam F Function type.
   * \tparam ARGS Argument types.
   * \param function Function to call for each pixel.
   * \param arguments Extra arguments forwarded to the function.
   */
  template<typename F, typename... ARGS>
  void Apply(F&& function, ARGS&&... arguments);

  /** Returns the width of the image in pixels.
   * \brief Returns width.
   * \return Width in pixels.
   */
  u32 GetWidth() const { return mWidth; }

  /** Returns the height of the image in pixels.
   * \brief Returns height.
   * \return Height in pixels.
   */
  u32 GetHeight() const { return mHeight; }

  /** Returns the format of the image.
   * \brief Returns format.
   * \return Image format.
   */
  Format GetFormat() const { return mFormat; }

  /** Returns the data of the image.
   * \brief Returns image data.
   * \return Image data.
   */
  u8* GetData() { return mData; }

  /** Returns the data of the image.
   * \brief Returns image data.
   * \return Image data.
   */
  const u8* GetData() const { return mData; }

public:
  /** Returns the number of bytes that are required to store a single pixel in
   * the specified format
   * \brief Returns bytes per pixel for format.
   * \param format Format to get bytes per pixel for.
   * \return Bytes per pixel.
   */
  static u32 BytesPerPixel(Format format);

  /** Returns the number of channels for an image format.
   * \brief Returns channel count.
   * \param format Format to get channel count.
   * \return Channel count for format.
   */
  static u32 ChannelCount(Format format);

  /** Returns whether or not the specified image format has a dedicated alpha
   * channel.
   * \brief Returns whether format has alpha channel.
   * \param format Format to check for.
   * \return True if the format has an alpha channel otherwise false.
   */
  static bool HasAlphaChannel(Format format);

  /** Grayscale filter function that converts the color of the pixel to black
   * and white.
   * \brief Grayscale filter.
   * \param image Image to filter pixel in.
   * \param x Pixel x position.
   * \param y Pixel y position.
   * \return Color after conversion.
   */
  static Color GrayscaleFilter(Image& image, u32 x, u32 y);

  /** Output stream function to correctly print the names of image results.
   * \brief Output result names.
   * \param os Output stream.
   * \param result Result to output.
   * \return Output stream.
   */
  friend std::ostream& operator<<(std::ostream& os, const Result& result);

private:
  /** Load image data from the path into the specified image. This is only used
   * internally to load the data into the image.
   * \brief Load image data.
   * \param image Image to load data into.
   * \param path Path to load image from.
   * \param allocator Allocator to allocate image data with.
   * \return Result.
   * - Result::kSuccess: Successfully loaded image from file.
   */
  static Result LoadImageData(
    Image& image,
    const Path& path,
    Allocator& allocator = DefaultAllocator::Instance());
};

// -------------------------------------------------------------------------- //

template<typename F, typename... ARGS>
void
Image::Apply(F&& function, ARGS&&... arguments)
{
  for (u32 y = 0; y < mHeight; y++) {
    for (u32 x = 0; x < mWidth; x++) {
      Color color = function(*this, x, y, std::forward<ARGS>(arguments)...);
      SetPixel(x, y, color);
    }
  }
}

}