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

#include "alflib/file/archive.hpp"
#include <alflib/file/archive.hpp>

// ========================================================================== //
// Headers
// ========================================================================== //

// Project headers
#include "alflib/file/file.hpp"

// ========================================================================== //
// Functions
// ========================================================================== //

namespace alflib {

FileResult
FromMinizResult(mz_zip_error error)
{
  switch (error) {

    case MZ_ZIP_NO_ERROR:
      return FileResult::kSuccess;
    case MZ_ZIP_UNDEFINED_ERROR:
      return FileResult::kUnknownError;
    case MZ_ZIP_FILE_OPEN_FAILED:
      return FileResult::kNotFound;
    case MZ_ZIP_FILE_CLOSE_FAILED:
      return FileResult::kNotOpen;
    case MZ_ZIP_INVALID_PARAMETER:
      return FileResult::kInvalidArgument;
    case MZ_ZIP_TOO_MANY_FILES:
    case MZ_ZIP_FILE_TOO_LARGE:
    case MZ_ZIP_UNSUPPORTED_METHOD:
    case MZ_ZIP_UNSUPPORTED_ENCRYPTION:
    case MZ_ZIP_UNSUPPORTED_FEATURE:
    case MZ_ZIP_FAILED_FINDING_CENTRAL_DIR:
    case MZ_ZIP_NOT_AN_ARCHIVE:
    case MZ_ZIP_INVALID_HEADER_OR_CORRUPTED:
    case MZ_ZIP_UNSUPPORTED_MULTIDISK:
    case MZ_ZIP_DECOMPRESSION_FAILED:
    case MZ_ZIP_COMPRESSION_FAILED:
    case MZ_ZIP_UNEXPECTED_DECOMPRESSED_SIZE:
    case MZ_ZIP_CRC_CHECK_FAILED:
    case MZ_ZIP_UNSUPPORTED_CDIR_SIZE:
    case MZ_ZIP_ALLOC_FAILED:
    case MZ_ZIP_FILE_CREATE_FAILED:
    case MZ_ZIP_FILE_WRITE_FAILED:
    case MZ_ZIP_FILE_READ_FAILED:
    case MZ_ZIP_FILE_SEEK_FAILED:
    case MZ_ZIP_FILE_STAT_FAILED:
    case MZ_ZIP_INVALID_FILENAME:
    case MZ_ZIP_BUF_TOO_SMALL:
    case MZ_ZIP_INTERNAL_ERROR:
    case MZ_ZIP_FILE_NOT_FOUND:
    case MZ_ZIP_ARCHIVE_TOO_LARGE:
    case MZ_ZIP_VALIDATION_FAILED:
    case MZ_ZIP_WRITE_CALLBACK_FAILED:
    case MZ_ZIP_TOTAL_ERRORS:
    default:
      return FileResult::kUnknownError;
  }
}

}

// ========================================================================== //
// Archive Implementation
// ========================================================================== //

namespace alflib {

Archive::Archive(File file, Type type)
  : mFile(file)
  , mIsOpen(false)
  , mData({})
{
  // Determine archive type if unknown
  if (type == Type::kUnknown) {
    Path::Extension extension = file.GetPath().GetExtension();
    if (extension == Path::Extension::kTar) {
      type = Type::kTar;
    } else if (extension == Path::Extension::kZip) {
      type = Type::kZip;
    }
  }
  AlfAssert(type != Type::kUnknown,
            "Archive type must be either given or found by looking at path");
  mType = type;
}

// -------------------------------------------------------------------------- //

Archive::~Archive()
{
  if (mIsOpen) {
    Close();
  }
}

// -------------------------------------------------------------------------- //

FileResult
Archive::Open()
{
  // Make sure it's not already open
  if (mIsOpen) {
    return FileResult::kAlreadyOpen;
  }

  // Handle specific archives
  if (mType == Type::kTar) {
    Path absolute = mFile.GetPath().GetAbsolutePath();
    int result = mtar_open(&mData.tar, absolute.GetPathString().GetUTF8(), "r");
    if (result != MTAR_ESUCCESS) {
      return FileResult::kUnknownError;
    }
  }
  if (mType == Type::kZip) {
    // TODO(Filip Bj�rklund): Do this correctly!
    mData.zip.reader = {};
    mData.zip.writer = {};
    const mz_bool success = mz_zip_reader_init_file(
      &mData.zip.reader, mFile.GetPath().GetPathString().GetUTF8(), 0);
    if (!success) {
      mz_zip_error error = mz_zip_get_last_error(&mData.zip.reader);
      return FromMinizResult(error);
    }
  }

  // Finalize
  mIsOpen = true;
  return FileResult::kSuccess;
}

// -------------------------------------------------------------------------- //

FileResult
Archive::Close()
{
  // Check that archive is open
  if (!mIsOpen) {
    return FileResult::kNotOpen;
  }

  // Handle specific archives
  if (mType == Type::kTar) {
    mtar_close(&mData.tar);
  }
  if (mType == Type::kZip) {
    // TODO(Filip Bj�rklund): Fix reader/write
    mz_zip_end(&mData.zip.reader);
    // mz_zip_end(&mData.zip.writer);
  }

  // Finalize
  mIsOpen = false;
  return FileResult::kSuccess;
}

// -------------------------------------------------------------------------- //

ArrayList<File>
Archive::Enumerate()
{
  AlfAssert(mIsOpen, "Archive must be open to enumerate");

  ArrayList<File> files;

  // Handle specific archives
  if (mType == Type::kTar) {
    mtar_header_t header{};
    while (mtar_read_header(&mData.tar, &header) != MTAR_ENULLRECORD) {
      files.AppendEmplace(header.name);
      mtar_next(&mData.tar);
    }
  }
  if (mType == Type::kZip) {
    const mz_uint count = mz_zip_reader_get_num_files(&mData.zip.reader);
    for (mz_uint i = 0; i < count; i++) {
      mz_zip_archive_file_stat stats;
      if (mz_zip_reader_file_stat(&mData.zip.reader, i, &stats)) {
        files.AppendEmplace(stats.m_filename);
      }
    }
  }

  return files;
}

}
