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

// Library headers
#include <microtar/microtar.h>
#include <miniz/miniz.h>

// Project headers
#include "alflib/collection/array_list.hpp"
#include "alflib/file/file.hpp"
#include "alflib/file/result.hpp"
#include "alflib/core/macros.hpp"

// ========================================================================== //
// Archive Declaration
// ========================================================================== //

namespace alflib {

/** \class Archive
 * \author Filip Bj�rklund
 * \date 11 juni 2019 - 15:11
 * \brief Archive.
 * \details
 * Represents an archive that is a collection of files in the file system. An
 * archive can also optionally be compressed to take up less space than the
 * individual files would.
 */
class Archive
{
  ALFLIB_CLASS_NON_COPYABLE(Archive)

public:
  /** Archive type **/
  enum class Type
  {
    /** Unknown type **/
    kUnknown,
    /** Tar **/
    kTar,
    /** Zip **/
    kZip
  };

private:
  /** File **/
  File mFile;
  /** Type of the archive **/
  Type mType;

  /** Whether archive is open **/
  bool mIsOpen;
  /** Archive data union */
  union
  {
    /** Tar archive **/
    mtar_t tar;
    /** Zip archive**/
    struct
    {
      /** Reader **/
      mz_zip_archive reader;
      /** Writer **/
      mz_zip_archive writer;
    } zip;
  } mData;

public:
  /** Construct an archive from a file and an archive type. If the archive type
   * is unknown then the type must be able to be determined from the file.
   * \brief Construct archive.
   * \param file File that represents archive.
   * \param type Type of the archive.
   */
  explicit Archive(File file, Type type = Type::kUnknown);

  /** Destruct archive and also close it if it's still open.
   * \brief Destruct.
   */
  ~Archive();

  /** Open archive to be able to perform operations on files in it and also t
   * add and remove files.
   * \brief Open archive.
   * \return Result.
   */
  FileResult Open();

  /** Close archive.
   * \brief Close archive.
   * \return Result.
   */
  FileResult Close();

  /** Enumerate all the files in the archive.
   * \brief Enumerate files.
   * \return List of files.
   */
  ArrayList<File> Enumerate();
};

}