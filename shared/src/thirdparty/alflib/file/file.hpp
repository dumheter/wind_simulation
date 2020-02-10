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
#include "alflib/collection/array_list.hpp"
#include "alflib/core/common.hpp"
#include "alflib/file/path.hpp"
#include "alflib/file/result.hpp"
#include "alflib/platform/platform.hpp"

// ========================================================================== //
// File Declaration
// ========================================================================== //

namespace alflib {

/** \class File
 * \author Filip Björklund
 * \date 07 juni 2019 - 20:31
 * \brief File handle.
 * \details
 * Handle to a file system object. This can represent files, directories,
 * archives and non-existing objects.
 *
 * When a file represents a read file it can be operated on by creating a FileIO
 * object.
 *
 * When the file represents a directory or an archive it can be enumerated.
 */
class File
{
private:
  /** File path **/
  Path mPath;

#if defined(ALFLIB_TARGET_WINDOWS)
  /** Win32 file attributes **/
  WIN32_FILE_ATTRIBUTE_DATA mFileAttributes;
#else
  /** Posix stat **/
  struct stat mStats;
  /** Whether stats are valid **/
  bool mValidStats = false;
#endif

public:
  /** Enumeration of file system object types **/
  enum class Type
  {
    /** Invalid type. This is the type of files that does not exist **/
    kInvalid,
    /** File **/
    kFile,
    /** Directory **/
    kDirectory,
    /** Archive **/
    kArchive
  };

public:
  /** Construct a file handle that represents the object at the specified path
   * in the filesystem.
   * \note This operation does not actually open the file for reading. Instead
   * File::OpenFile() function should be used to perform operations on files.
   * \brief Create file.
   * \param path Path to file.
   */
  File(const Path& path);

  /** Create a file handle that represents the object at the specified path in
   * the filesystem.
   * \note This operation does not actually open the file for reading. Instead
   * File::OpenFile() function should be used to perform operations on files.
   * \brief Create file.
   * \param path Path to file.
   */
  File(const String& path);

  /** Destruct file handle **/
  ~File() = default;

  /** Open a file relative to the current file.
   * \brief Open relative file.
   * \return File.
   */
  File Open(const Path& path) const;

  /** Open a file relative to the current file.
   * \brief Open relative file.
   * \return File.
   */
  File Open(const String& path) const;

  /** Returns a file that represents a sibling to this file. The sibling will
   * be opened relative to the directory of this file.
   * \brief Returns sibling.
   * \param path Path to sibling relative to this file.
   * \return Sibling file.
   */
  File Sibling(const String& path);

  /** Create the file system object. The type of the object to create is
   * specified and files created can optionally overwrite any existing files at
   * the path.
   * \brief Create file.
   * \param type Type of file system object to create.
   * \param overwrite Whether to overwrite any existing files at the path.
   * \return Result.
   */
  FileResult Create(Type type, bool overwrite = false);

  /** Delete the file system object.
   * \brief Delete file.
   * \param recursive Whether to recursively delete subdirectories.
   * \return Result.
   */
  FileResult Delete(bool recursive = false);

  /** Rename the file to the specified name. The name should only be the name of
   * the file and extension, not the entire path.
   * \brief Rename file.
   * \param name New name.
   * \return Result.
   */
  FileResult Rename(const String& name);

  /** Copy the file to a new path.
   * \brief Copy file.
   * \param to Path to copy to.
   * \return Result.
   */
  FileResult Copy(const Path& to);

  /** Move the file to a new path.
   * \brief Move file.
   * \param to Path to move to.
   * \return Result.
   */
  FileResult Move(const Path& to);

  /** Returns whether there exists a file system object at the path of this
   * file.
   * \brief Returns whether file exists.
   * \return True if there exists and object at the path otherwise false.
   */
  bool Exists() const;

  /** Enumerate all the files in a directory or archive. Optionally the user may
   * choose to not include any special directory names.
   * \pre Type of file must be a directory.
   * \brief Enumerate files in directory or archive.
   * \param includeSpecial Whether special directories should be included. On
   * most platforms these are '..', '.' and '~'.
   * \return List of files.
   */
  ArrayList<File> Enumerate(bool includeSpecial = true) const;

  /** Returns the path to the file.
   * \brief Returns path.
   * \return Path to file.
   */
  const Path& GetPath() const { return mPath; }

  /** Returns the type of the file. It's important that archive files have the
   * correct file extensions to make sure that the types are correctly
   * determined on all platforms.
   * \brief Returns file type.
   * \return Type of the file.
   */
  Type GetType() const;

  /** Returns the size of the file.
   * \brief Returns file size.
   * \return Size of the file.
   */
  u64 GetSize() const;

  /** Returns whether or not two files are equal.
   * \brief Returns whether files are equal.
   * \param file0 First file.
   * \param file1 Second file.
   * \return True if the files are equal otherwise false.
   */
  friend bool operator==(const File& file0, const File& file1);

  /** Returns whether or not two file are not equal.
   * \brief Returns whether files are unequal.
   * \param file0 First file.
   * \param file1 Second file.
   * \return True if the files are not equal otherwise false.
   */
  friend bool operator!=(const File& file0, const File& file1);

private:
  /** Retrieve current file attributes **/
  void UpdateAttributes();
};

}