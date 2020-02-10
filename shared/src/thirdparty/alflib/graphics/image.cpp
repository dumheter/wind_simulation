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

#include "alflib/graphics/image.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Project headers
#include "alflib/core/assert.hpp"
#include "alflib/file/file_io.hpp"

// stb_image header
#define STBI_ASSERT(x) AlfAssert(x, "Assertion failed in library stb_image")
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

// stb_image_write header
#define STBIW_ASSERT(x) AlfAssert(x, "Assertion failed in library stb_image")
#define STB_IMAGE_WRITE_IMPLEMENTATION
#if defined(ALFLIB_TARGET_WINDOWS)
#define STBI_MSC_SECURE_CRT
#endif
#include <stb/stb_image_write.h>

// stb_image_resize header
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_ASSERT(x)                                                        \
  AlfAssert(x, "Assertion failed in library stb_image_resize")
#include <stb/stb_image_resize.h>

// ========================================================================== //
// Structures
// ========================================================================== //

namespace alflib {

/** Context structure used during image saving **/
struct ImageSaveContext {
  /** IO to write data with **/
  FileIO *io;
  /** Output result of operation **/
  Image::Result result;
};

} // namespace alflib

// ========================================================================== //
// Functions
// ========================================================================== //

namespace alflib {

static void ImageSaveCallback(ImageSaveContext *context, void *data, s32 size) {
  u64 written;
  FileResult fileResult = context->io->Write(reinterpret_cast<const u8 *>(data),
                                             static_cast<u64>(size), written);
  if (fileResult != FileResult::kSuccess || written != static_cast<u64>(size)) {
    context->result = Image::Result::kFailedToWriteFile;
    return;
  }
  context->result = Image::Result::kSuccess;
}

// -------------------------------------------------------------------------- //

static stbir_filter ToSTBIRFilter(Image::Filter filter) {
  switch (filter) {
  case Image::Filter::kBox:
    return STBIR_FILTER_BOX;
  case Image::Filter::kTriangle:
    return STBIR_FILTER_TRIANGLE;
  case Image::Filter::kCubicSpline:
    return STBIR_FILTER_CUBICBSPLINE;
  case Image::Filter::kCatmullRom:
    return STBIR_FILTER_CATMULLROM;
  case Image::Filter::kMitchell:
    return STBIR_FILTER_MITCHELL;
  default:
    return STBIR_FILTER_DEFAULT;
  }
}

} // namespace alflib

// ========================================================================== //
// Image Implementation
// ========================================================================== //

namespace alflib {

Image::Image(Allocator &allocator) : mAllocator(allocator) {}

// -------------------------------------------------------------------------- //

Image::Image(const Image &other)
    : mWidth(other.mWidth), mHeight(other.mHeight), mFormat(other.mFormat),
      mAllocator(other.mAllocator) {
  if (other.mData) {
    mData = static_cast<u8 *>(mAllocator.Alloc(mDataSize));
    memcpy(mData, other.mData, mDataSize);
    mDataSize = other.mDataSize;
  }
}

// -------------------------------------------------------------------------- //

Image::Image(Image &&other)
    : mWidth(other.mWidth), mHeight(other.mHeight), mFormat(other.mFormat),
      mAllocator(other.mAllocator), mData(other.mData),
      mDataSize(other.mDataSize) {
  other.mData = nullptr;
  other.mDataSize = 0;
}

// -------------------------------------------------------------------------- //

Image::~Image() {
  mAllocator.Delete(mData);
  mData = nullptr;
  mDataSize = 0;
}

// -------------------------------------------------------------------------- //

Image &Image::operator=(const Image &other) {
  if (this != &other) {
    mAllocator.Free(mData);

    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mFormat = other.mFormat;
    mAllocator = other.mAllocator;
    mData = other.mData;
    mDataSize = other.mDataSize;
    if (other.mData) {
      mData = static_cast<u8 *>(mAllocator.Alloc(mDataSize));
      memcpy(mData, other.mData, mDataSize);
      mDataSize = other.mDataSize;
    }
  }
  return *this;
}

// -------------------------------------------------------------------------- //

Image &Image::operator=(Image &&other) {
  if (this != &other) {
    mAllocator.Free(mData);

    mWidth = other.mWidth;
    mHeight = other.mHeight;
    mFormat = other.mFormat;
    mAllocator = other.mAllocator;
    mData = other.mData;
    mDataSize = other.mDataSize;
    other.mData = nullptr;
    other.mDataSize = 0;
  }
  return *this;
}

// -------------------------------------------------------------------------- //

Image::Result Image::Load(const Path &path, Image::Format format) {
  // Load image data
  Image sourceImage;
  Result result = LoadImageData(sourceImage, path, mAllocator);
  if (result != Result::kSuccess) {
    return result;
  }

  // Convert if necessary
  if (mFormat != format) {
    sourceImage.ConvertedTo(format, *this);
  } else {
    operator=(sourceImage);
  }

  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Image::Result Image::Create(u32 width, u32 height, Image::Format format,
                            const u8 *data, Image::Format dataFormat) {
  // Free any previous data
  mAllocator.Free(mData);
  mData = nullptr;
  mDataSize = 0;

  // Setup image info
  mWidth = width;
  mHeight = height;
  mFormat = format;

  // Setup data
  if (format == dataFormat || dataFormat == Format::kUnknown) {
    mDataSize = mWidth * mHeight * BytesPerPixel(mFormat);
    mData = mAllocator.NewArray<u8>(mDataSize);
    if (data) {
      memcpy(mData, data, mDataSize);
    }
  } else {
    Image sourceImage;
    Result result = sourceImage.Create(width, height, dataFormat, data);
    if (result != Result::kSuccess) {
      return result;
    }
    operator=(sourceImage);
  }

  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Image::Result Image::ConvertedTo(Image::Format targetFormat, Image &image) {
  // Clear input image
  image.mAllocator.Delete(image.mData);
  image.mDataSize = 0;

  // If the format is unchanged then return a copy
  if (mFormat == targetFormat) {
    image = *this;
    return Result::kSuccess;
  }

  // Create image data
  Result result = image.Create(mWidth, mHeight, targetFormat);
  if (result != Result::kSuccess) {
    return result;
  }

  // Do pixel-by-pixel (slow) copy
  for (u32 y = 0; y < mHeight; y++) {
    for (u32 x = 0; x < mWidth; x++) {
      image.SetPixel(x, y, GetPixel(x, y));
    }
  }
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Image::Result Image::ResizedTo(u32 width, u32 height, Image::Filter filter,
                               Image &image) {
  image.mAllocator.Delete(image.mData);
  image.mData = nullptr;
  image.mDataSize = 0;

  // Allocate buffer for resized data
  u64 bufferSize = static_cast<u64>(width) * static_cast<u64>(height) *
                   static_cast<u64>(BytesPerPixel(mFormat));
  u8 *buffer = mAllocator.NewArray<u8>(bufferSize);

  // Resize image
  stbir_resize_uint8_generic(mData, mWidth, mHeight, 0, buffer, width, height,
                             0, ChannelCount(mFormat), HasAlphaChannel(mFormat),
                             0, STBIR_EDGE_CLAMP, ToSTBIRFilter(filter),
                             STBIR_COLORSPACE_LINEAR, nullptr);

  // Assign new data and return
  image.mWidth = width;
  image.mHeight = height;
  image.mFormat = mFormat;
  image.mAllocator = mAllocator;
  image.mData = buffer;
  image.mDataSize = bufferSize;
  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Image ::Result Image::Blit(const Image &srcImage, u32 dstOffsetX,
                           u32 dstOffsetY, u32 srcOffsetX, u32 srcOffsetY,
                           u32 width, u32 height) {
  // Set the width and height if the special values (U32_MAX) has been specified
  if (width == U32_MAX) {
    width = srcImage.mWidth;
  }
  if (height == U32_MAX) {
    height = srcImage.mHeight;
  }

  // Assert preconditions
  AlfAssert(srcOffsetX + width <= srcImage.mWidth,
            "Source image X offset plus width must not be greater than "
            "the width of the source image");
  AlfAssert(srcOffsetY + height <= srcImage.mHeight,
            "Source image Y offset plus height must not be greater than "
            "the height of the source image");
  AlfAssert(dstOffsetX + width <= mWidth,
            "Destination image X offset plus width must not be greater than "
            "the width of the destination image");
  AlfAssert(dstOffsetY + height <= mHeight,
            "Destination image Y offset plus height must not be greater than "
            "the height of the destination image");

  // Fast-path?
  if (mFormat == srcImage.mFormat) {
    // Copy rows
    for (u32 row = 0; row < height; row++) {
      u8 *dstPointer = mData + BytesPerPixel(mFormat) *
                                   (mWidth * (row + dstOffsetY) + dstOffsetX);
      u8 *srcPointer = srcImage.mData +
                       BytesPerPixel(mFormat) *
                           (srcImage.mWidth * (srcOffsetY + row) + srcOffsetX);
      memcpy(dstPointer, srcPointer, BytesPerPixel(mFormat) * width);
    }
  } else {
    // Otherwise slow path (pixel-by-pixel)
    for (u32 y = 0; y < height; y++) {
      for (u32 x = 0; x < width; x++) {
        SetPixel(x + dstOffsetX, y + dstOffsetY,
                 srcImage.GetPixel(x + srcOffsetX, y + srcOffsetY));
      }
    }
  }

  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

void Image::Fill(const Color &color) {
  Apply([&color](Image &, u32, u32) { return color; });
}

// -------------------------------------------------------------------------- //

Image::Result Image::Save(const Path &path, bool overwrite) {
  // Determine extension and its validity
  Path::Extension extension = path.GetExtension();
  if (extension != Path::Extension::kPng &&
      extension != Path::Extension::kTga &&
      extension != Path::Extension::kJpeg) {
    return Result::kInvalidFileFormat;
  }

  // Open file
  File imageFile(path);
  if (imageFile.Exists() && !overwrite) {
    return Result::kImageFileAlreadyExists;
  }
  FileIO io(imageFile);
  FileIO::Flag ioFlags =
      FileIO::Flag::kCreate | FileIO::Flag::kRead | FileIO::Flag::kShareRead;
  if (overwrite) {
    ioFlags |= FileIO::Flag::kOverwrite;
  }
  FileResult fileResult = io.Open(ioFlags);
  if (fileResult != FileResult::kSuccess) {
    return Result::kFailedToWriteFile;
  }

  // Save the file
  ImageSaveContext context{&io, Result::kSuccess};
  u32 bytesPerPixel = BytesPerPixel(mFormat);
  u32 channelCount = ChannelCount(mFormat);
  switch (extension) {
  // Handle: PNG
  case Path::Extension::kPng: {
    stbi_write_png_to_func(
        reinterpret_cast<stbi_write_func *>(ImageSaveCallback), &context,
        mWidth, mHeight, channelCount, mData, bytesPerPixel * mWidth);
    if (context.result != Result::kSuccess) {
      return context.result;
    }
    break;
  }
  // Handle: TGA
  case Path::Extension::kTga: {
    stbi_write_tga_to_func(
        reinterpret_cast<stbi_write_func *>(ImageSaveCallback), &context,
        mWidth, mHeight, channelCount, mData);
    if (context.result != Result::kSuccess) {
      return context.result;
    }
    break;
  }
  // Handle: JPEG
  case Path::Extension::kJpeg: {
    stbi_write_jpg_to_func(
        reinterpret_cast<stbi_write_func *>(ImageSaveCallback), &context,
        mWidth, mHeight, channelCount, mData, 0);
    if (context.result != Result::kSuccess) {
      return context.result;
    }
    break;
  }
  default:
    break;
  }

  return Result::kSuccess;
}

// -------------------------------------------------------------------------- //

Color Image::GetPixel(u32 x, u32 y) const {
  // Assert precondition
  AlfAssert(mFormat != Format::kUnknown,
            "'Image::GetPixel()' is not valid for an image with the format "
            "'Format::kUnknown'");

  // Set pixel value
  const u64 _x = static_cast<u64>(x);
  const u64 _y = static_cast<u64>(y);
  const u32 bytesPerPixel = BytesPerPixel(mFormat);
  u8 *address = mData + (_y * mWidth * bytesPerPixel) + (_x * bytesPerPixel);
  switch (mFormat) {
  case Format::kGrayscale: {
    return Color{address[0], address[0], address[0]};
  }
  case Format::kRGBA: {
    return Color{address[0], address[1], address[2], address[3]};
  }
  case Format::kRGB: {
    return Color{address[0], address[1], address[2]};
  }
  case Format::kBGRA: {
    return Color{address[2], address[1], address[0], address[3]};
  }
  case Format::kBGR: {
    return Color{address[2], address[1], address[0]};
  }
  case Format::kUnknown:
  default:
    return Color{0.0f, 0.0f, 0.0f};
  }
}

// -------------------------------------------------------------------------- //

void Image::SetPixel(u32 x, u32 y, const Color &color) {
  // Assert precondition
  AlfAssert(mFormat != Format::kUnknown,
            "'Image::SetPixel()' is not valid for an image with the format "
            "'Format::kUnknown'");

  // Retrieve pixel value
  const u64 _x = static_cast<u64>(x);
  const u64 _y = static_cast<u64>(y);
  const u32 bytesPerPixel = BytesPerPixel(mFormat);
  u8 *address = mData + (_y * mWidth * bytesPerPixel) + (_x * bytesPerPixel);
  switch (mFormat) {
  case Format::kGrayscale: {
    address[0] = color.Red();
    break;
  }
  case Format::kRGBA: {
    address[0] = color.Red();
    address[1] = color.Green();
    address[2] = color.Blue();
    address[3] = color.Alpha();
    break;
  }
  case Format::kRGB: {
    address[0] = color.Red();
    address[1] = color.Green();
    address[2] = color.Blue();
    break;
  }
  case Format::kBGRA: {
    address[2] = color.Red();
    address[1] = color.Green();
    address[0] = color.Blue();
    address[3] = color.Alpha();
    break;
  }
  case Format::kBGR: {
    address[2] = color.Red();
    address[1] = color.Green();
    address[0] = color.Blue();
    break;
  }
  case Format::kUnknown:
  default:
    break;
  }
}

// -------------------------------------------------------------------------- //

u32 Image::BytesPerPixel(Image::Format format) {
  switch (format) {
  case Format::kUnknown:
    return 0;
  case Format::kGrayscale:
    return 1;
  case Format::kRGBA:
    return 4;
  case Format::kRGB:
    return 3;
  case Format::kBGRA:
    return 4;
  case Format::kBGR:
    return 3;
  default:
    return 0;
  }
}

// -------------------------------------------------------------------------- //

u32 Image::ChannelCount(Image::Format format) {
  switch (format) {
  case Format::kUnknown:
    return 0;
  case Format::kGrayscale:
    return 1;
  case Format::kRGBA:
    return 4;
  case Format::kRGB:
    return 3;
  case Format::kBGRA:
    return 4;
  case Format::kBGR:
    return 3;
  default:
    return 0;
  }
}

// -------------------------------------------------------------------------- //

bool Image::HasAlphaChannel(Image::Format format) {
  return format == Image::Format::kRGBA || format == Image::Format ::kBGRA;
}

// -------------------------------------------------------------------------- //

Color Image::GrayscaleFilter(Image &image, u32 x, u32 y) {
  return image.GetPixel(x, y).ToGrayscale();
}

// -------------------------------------------------------------------------- //

std::ostream &operator<<(std::ostream &os, const Image::Result &result) {
  switch (result) {
  case Image::Result::kSuccess: {
    os << "kSuccess";
    break;
  }
  case Image::Result::kOutOfMemory: {
    os << "kOutOfMemory";
    break;
  }
  case Image::Result::kFileNotFound: {
    os << "kFileNotFound";
    break;
  }
  case Image::Result::kInvalidFileFormat: {
    os << "kInvalidFileFormat";
    break;
  }
  case Image::Result::kFailedToWriteFile: {
    os << "kFailedToWriteFile";
    break;
  }
  case Image::Result ::kImageFileAlreadyExists: {
    os << "kImageFileAlreadyExists";
    break;
  }
  }
  return os;
}

// -------------------------------------------------------------------------- //

Image::Result Image::LoadImageData(Image &image, const Path &path,
                                   Allocator &allocator) {
  // Open image file
  File imageFile(path);
  FileIO io(imageFile);
  FileResult fileResult =
      io.Open(FileIO::Flag::kRead | FileIO::Flag::kShareRead);
  if (fileResult != FileResult::kSuccess) {
    return Image::Result::kFileNotFound;
  }

  // Read file
  u64 size = imageFile.GetSize();
  u64 read;
  u8 *buffer = allocator.NewArray<u8>(size);
  fileResult = io.Read(buffer, size, read);
  if (fileResult != FileResult::kSuccess) {
    allocator.Delete(buffer);
    return Image::Result::kFileNotFound;
  }

  // Parse file
  s32 x, y, c;
  stbi_uc *imageData =
      stbi_load_from_memory(buffer, int(size), &x, &y, &c, STBI_default);
  allocator.Delete(buffer);
  image.mWidth = x;
  image.mHeight = y;

  // Determine format and data size
  image.mFormat = Image::Format::kUnknown;
  if (c == 1) {
    image.mFormat = Image::Format::kGrayscale;
  }
  if (c == 3) {
    image.mFormat = Image::Format::kRGB;
  }
  if (c == 4) {
    image.mFormat = Image::Format::kRGBA;
  }
  image.mDataSize = x * y * Image::BytesPerPixel(image.mFormat);

  // Copy data to use correct allocator
  // TODO(Filip Björklund): Don't do this copy!
  image.mData = allocator.NewArray<u8>(image.mDataSize);
  memcpy(image.mData, imageData, image.mDataSize);
  stbi_image_free(imageData);

  return Image::Result::kSuccess;
}

} // namespace alflib