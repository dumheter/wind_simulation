// MIT License
//
// Copyright (c) 2019 Filip Bj�rklund
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

#include "alflib/file/file.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Library headers
#include "thirdparty/miniz/miniz.h"

// Project headers
#include "alflib/file/archive.hpp"

// ========================================================================== //
// Windows
// ========================================================================== //

#if defined(ALFLIB_TARGET_WINDOWS)

namespace alflib {

FileResult
FileErrorFromErrorWin32(DWORD error)
{
  switch (error) {
    case ERROR_SUCCESS:
      return FileResult::kSuccess;
    case ERROR_FILE_EXISTS:
      return FileResult::kAlreadyExists;
    case ERROR_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
      return FileResult::kNotFound;
    case ERROR_ACCESS_DENIED:
      return FileResult::kAccessDenied;
    default:
      return FileResult::kUnknownError;
  }
}

// -------------------------------------------------------------------------- //

static s64
LastOffsetOfWStringWin32(const wchar_t* str, wchar_t chr)
{
  s64 lastIndex = -1;
  for (size_t i = 0; i < wcslen(str); i++) {
    if (str[i] == chr) {
      lastIndex = i;
    }
  }
  return lastIndex;
}

// -------------------------------------------------------------------------- //

static DWORD
CreateDirectoryRecursivelyWin32(char16* path)
{
  // Try to create directory
  BOOL result = CreateDirectoryW(path, NULL);
  if (result == 0) {
    // Retrieve error
    const DWORD error = GetLastError();

    // Check if invalid path or other error
    if (error == ERROR_PATH_NOT_FOUND) {
      // Create sub directory
      // Support both '\' and '/' file separators
      const s64 lastIndex0 = LastOffsetOfWStringWin32(path, '\\');
      const s64 lastIndex1 = LastOffsetOfWStringWin32(path, '/');
      const s64 lastIndex = Max(lastIndex0, lastIndex1);

      char16 subPath[MAX_PATH];
      if (lastIndex < 0) {
        return ERROR_PATH_NOT_FOUND;
      }
      memcpy(subPath, path, sizeof(char16) * lastIndex);
      subPath[lastIndex] = 0;

      // Recursively create sub-path
      const DWORD recursiveResult = CreateDirectoryRecursivelyWin32(subPath);
      if (recursiveResult != ERROR_SUCCESS) {
        return recursiveResult;
      }

      // Create this directory
      result = CreateDirectoryW(path, NULL);
      return result == 0 ? GetLastError() : ERROR_SUCCESS;
    }

    // Return other error
    return error;
  }

  // Return success
  return ERROR_SUCCESS;
}

}

#endif

// ========================================================================== //
// Linux
// ========================================================================== //

#if defined(ALFLIB_TARGET_LINUX)

namespace alflib {

/** Convert to FileResult **/
static FileResult
FileErrorFromErrno(int error)
{
  switch (error) {
    // Flags are incorrect
    case EINVAL:
      return FileResult::kInvalidArgument;
    // Access was denied to file
    case EACCES:
      return FileResult::kAccessDenied;
    // File could not be opened as it does not exist
    case ENOENT:
      return FileResult::kNotFound;
    // Out of memory
    case ENOMEM:
    default:
      return FileResult::kUnknownError;
  }
}

}

#endif

// ========================================================================== //
// File Implementation
// ========================================================================== //

namespace alflib {

File::File(const Path& path)
  : mPath(path)
{
  UpdateAttributes();
}

// -------------------------------------------------------------------------- //

File::File(const String& path)
  : File(Path(path))
{}

// -------------------------------------------------------------------------- //

File
File::Open(const Path& path) const
{
  return File{ mPath.Joined(path) };
}

// -------------------------------------------------------------------------- //

File
File::Open(const String& path) const
{
  return File{ mPath.Joined(path) };
}

// -------------------------------------------------------------------------- //

File
File::Sibling(const String& path)
{
  return mPath.GetDirectory().Join(path);
}

// -------------------------------------------------------------------------- //

FileResult
File::Create(Type type, bool overwrite)
{
  if (type == Type::kInvalid) {
    return FileResult::kInvalidArgument;
  }

#if defined(ALFLIB_TARGET_WINDOWS)

  if (type == Type::kFile) {
    const auto _path = mPath.GetPathString().GetUTF16();
    const HANDLE file = CreateFileW(_path.Get(),
                                    0,
                                    0,
                                    NULL,
                                    overwrite ? OPEN_ALWAYS : CREATE_NEW,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
    if (file == INVALID_HANDLE_VALUE) {
      return FileErrorFromErrorWin32(GetLastError());
    }
    CloseHandle(file);
    return FileResult::kSuccess;
  }
  if (type == Type::kDirectory) {
    const DWORD result =
      CreateDirectoryRecursivelyWin32(mPath.GetPathString().GetUTF16().Get());
    return FileErrorFromErrorWin32(result);
  }

#else
  if (type == Type::kFile) {
    if (Exists() && !overwrite) {
      return FileResult::kAlreadyExists;
    }

    int flags = O_CREAT | O_RDWR;
    if (overwrite) {
      flags = O_TRUNC;
    } else {
      flags |= O_EXCL;
    }
    int result =
      open(mPath.GetPathString().GetUTF8(), flags, S_IRUSR | S_IWUSR);
    if (result < 0) {
      return FileErrorFromErrno(errno);
    }
    return FileResult::kSuccess;
  }
  if (type == Type::kDirectory) {
    int result = mkdir(mPath.GetPathString().GetUTF8(), 0);
    return FileErrorFromErrno(result);
  }
#endif

  // Handle archive
  if (type == Type::kArchive) {
    // TODO(Filip Bj�rklund): Create in memory and then create file instead?
    mz_zip_archive archive{};
    const mz_bool success =
      mz_zip_writer_init_file(&archive, mPath.GetPathString().GetUTF8(), 0);
    if (!success) {
      return FileResult::kAccessDenied;
    }
    return FileResult::kSuccess;
  }

  return FileResult::kSuccess;
}

// -------------------------------------------------------------------------- //

FileResult
File::Delete(bool recursive)
{
  if (!Exists()) {
    return FileResult::kNotFound;
  }

#if defined(ALFLIB_TARGET_WINDOWS)
  if (GetType() == Type::kFile || GetType() == Type::kArchive) {
    const auto path = mPath.GetPathString().GetUTF16();
    const BOOL success = DeleteFileW(path.Get());
    if (!success) {
      return FileErrorFromErrorWin32(GetLastError());
    }
    return FileResult::kSuccess;
  }
  if (GetType() == Type::kDirectory) {
    AlfBasicAssert(false, "Directories does not support being deleted yet");
  }
#else
  if (GetType() == Type::kFile || GetType() == Type::kArchive) {
    int result = remove(mPath.GetPathString().GetUTF8());
    if (result != 0) {
      return FileErrorFromErrno(errno);
    }
    return FileResult::kSuccess;
  } else if (GetType() == Type::kDirectory) {
    (void)recursive;
    AlfBasicAssert(false, "Directories does not support being deleted yet");
  }
#endif
  return FileResult::kUnknownError;
}

// -------------------------------------------------------------------------- //

FileResult
File::Rename(const String& name)
{
  // TODO(Filip Björklund): Implement
  (void)name;
  return FileResult::kUnknownError;
}

// -------------------------------------------------------------------------- //

FileResult
File::Copy(const Path& to)
{
  // TODO(Filip Björklund): Implement
  (void)to;
  return FileResult::kUnknownError;
}

// -------------------------------------------------------------------------- //

FileResult
File::Move(const Path& to)
{
  // TODO(Filip Björklund): Implement
  (void)to;
  return FileResult::kUnknownError;
}

// -------------------------------------------------------------------------- //

bool
File::Exists() const
{
#if defined(ALFLIB_TARGET_WINDOWS)
  return mFileAttributes.dwFileAttributes != INVALID_FILE_ATTRIBUTES;
#else
  return mValidStats;
#endif
}

// -------------------------------------------------------------------------- //

ArrayList<File>
File::Enumerate(bool includeSpecial) const
{
  // Assert preconditions
  AlfAssert(GetType() == Type::kDirectory || GetType() == Type::kArchive,
            "Only directories and archives can be enumerated");

  // List of files
  ArrayList<File> files;

#if defined(ALFLIB_TARGET_WINDOWS)
  // Add special directories if they should be included
  if (includeSpecial && GetType() == Type::kDirectory) {
    files.AppendEmplace(".");
    files.AppendEmplace("..");
  }

  // Enumerate and add files to list from directory
  if (GetType() == Type::kDirectory) {
    const auto path = (mPath.GetPathString() + "/*").GetUTF16();
    WIN32_FIND_DATAW findData;
    const HANDLE findHandle = FindFirstFileExW(path.Get(),
                                               FindExInfoBasic,
                                               &findData,
                                               FindExSearchNameMatch,
                                               nullptr,
                                               0);
    if (findHandle != INVALID_HANDLE_VALUE) {
      do {
        String filePath(findData.cFileName);
        if (filePath != "." && filePath != "..") {
          files.AppendEmplace(filePath);
        }
      } while (FindNextFileW(findHandle, &findData));
    }
    FindClose(findHandle);
  }
#else
  // Add special directories if they should be included
  if (includeSpecial && GetType() == Type::kDirectory) {
    files.AppendEmplace(".");
    files.AppendEmplace("..");
  }

  // Enumerate directory entries
  if (GetType() == Type::kDirectory) {
    DIR* directory = opendir(mPath.GetPathString().GetUTF8());
    if (directory) {
      struct dirent* entry;
      while ((entry = readdir(directory)) != nullptr) {
        String filePath(entry->d_name);
        if (filePath != "." && filePath != "..") {
          files.AppendEmplace(filePath);
        }
      }
      closedir(directory);
    }
  }
#endif

  // Handle archives
  if (GetType() == Type::kArchive) {
    Archive archive(*this);
    FileResult result = archive.Open();
    if (result == FileResult::kSuccess) {
      files = archive.Enumerate();
    }
  }

  // Return list of files
  return files;
}

// -------------------------------------------------------------------------- //

File::Type
File::GetType() const
{
  // Only existing files has a type
  if (!Exists()) {
    return Type::kInvalid;
  }

  // If the extension is of a known archive type then return archive
  if (mPath.GetExtension() == Path::Extension::kTar ||
      mPath.GetExtension() == Path::Extension::kZip) {
    return Type::kArchive;
  }

#if defined(ALFLIB_TARGET_WINDOWS)
  if (mFileAttributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    return Type::kDirectory;
  }
  if (mFileAttributes.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
    // TODO(Filip Bj�rklund): In documentation this looks incorrect. However it
    // does work
    return Type::kArchive;
  }
  return Type::kFile;
#else
  if (S_ISREG(mStats.st_mode)) {
    return Type::kFile;
  }
  if (S_ISDIR(mStats.st_mode)) {
    return Type::kDirectory;
  }
  return Type::kInvalid;
#endif
}

// -------------------------------------------------------------------------- //

u64
File::GetSize() const
{
  if (!Exists()) {
    return 0;
  }

#if defined(ALFLIB_TARGET_WINDOWS)
  const u64 sizeHigh = (uint64_t)mFileAttributes.nFileSizeHigh << sizeof(DWORD);
  const u64 sizeLow = mFileAttributes.nFileSizeLow;
  return sizeLow | sizeHigh;
#else
  return static_cast<u64>(mStats.st_size);
#endif
}

// -------------------------------------------------------------------------- //

void
File::UpdateAttributes()
{
#if defined(ALFLIB_TARGET_WINDOWS)
  const UniquePointer<char16[]> path = mPath.GetPathString().GetUTF16();
  GetFileAttributesExW(path.Get(), GetFileExInfoStandard, &mFileAttributes);
#else
  int result = stat(mPath.GetPathString().GetUTF8(), &mStats);
  mValidStats = result == 0;
#endif
}

// -------------------------------------------------------------------------- //

bool
operator==(const File& file0, const File& file1)
{
  return file0.mPath == file1.mPath;
}

// -------------------------------------------------------------------------- //

bool
operator!=(const File& file0, const File& file1)
{
  return file0.mPath != file1.mPath;
}

}
