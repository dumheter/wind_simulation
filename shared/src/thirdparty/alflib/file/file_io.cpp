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

#include "alflib/file/file_io.hpp"

// ========================================================================== //
// Windows
// ========================================================================== //

#if defined(ALFLIB_TARGET_WINDOWS)

namespace alflib {

FileResult
FileIOErrorFromErrorWin32(DWORD error)
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

}

#endif

// ========================================================================== //
// Linux
// ========================================================================== //

#if defined(ALFLIB_TARGET_LINUX)

namespace alflib {

/** Convert to FileResult **/
static FileResult
FileIOErrorFromErrno(int error)
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
// FileIO Implementation
// ========================================================================== //

namespace alflib {

FileIO::FileIO(File file)
  : mFile(file)
{}

// -------------------------------------------------------------------------- //

FileIO::FileIO(const Path& path)
  : mFile(File{ path })
{}

// -------------------------------------------------------------------------- //

FileIO::FileIO(const String& path)
  : mFile(File{ path })
{}

// -------------------------------------------------------------------------- //

FileIO::~FileIO()
{
  if (IsOpen()) {
    Close();
  }
}

// -------------------------------------------------------------------------- //

FileResult
FileIO::Open(Flag flags)
{
  // Check that file is not already open
  if (mIsOpen) {
    return FileResult::kAlreadyOpen;
  }

#if defined(ALFLIB_TARGET_WINDOWS)
  // Access flags
  DWORD access = 0;
  if (bool(flags & Flag::kRead)) {
    access |= GENERIC_READ;
  }
  if (bool(flags & Flag::kWrite)) {
    access |= GENERIC_WRITE;
  }

  // Sharing mode
  DWORD sharing = 0;
  if (bool(flags & Flag::kShareRead)) {
    sharing |= FILE_SHARE_READ;
  }
  if (bool(flags & Flag::kShareWrite)) {
    sharing |= FILE_SHARE_WRITE;
  }

  // Disposition
  const bool overwrite = bool(flags & Flag::kOverwrite);
  DWORD disposition = overwrite ? TRUNCATE_EXISTING : OPEN_EXISTING;
  if (bool(flags & Flag::kCreate)) {
    disposition = overwrite ? CREATE_ALWAYS : OPEN_ALWAYS;
  }

  // Attributes
  const DWORD attributes = FILE_ATTRIBUTE_NORMAL;

  // Open handle
  const auto path = mFile.GetPath().GetPathString().GetUTF16();
  mFileHandle = CreateFileW(
    path.Get(), access, sharing, NULL, disposition, attributes, NULL);
  if (mFileHandle == INVALID_HANDLE_VALUE) {
    const DWORD error = GetLastError();
    return FileIOErrorFromErrorWin32(error);
  }
#else
  // Create file or check that it exists
  if (bool(flags & Flag::kCreate)) {
    FileResult result = mFile.Create(File::Type::kFile);
    if (result != FileResult::kSuccess &&
        result != FileResult::kAlreadyExists) {
      return result;
    }
  } else if (!mFile.Exists()) {
    return FileResult::kNotFound;
  }

  // Determine correct mode
  const char* mode;
  if (bool(flags & Flag::kAppend)) {
    mode = "a+";
  } else if (bool(flags & Flag::kOverwrite)) {
    mode = "w+";
  } else {
    mode = "r+";
  }

  // Open handle
  const auto path = mFile.GetPath().GetPathString().GetUTF8();
  mFileHandle = fopen(path, mode);
  if (!mFileHandle) {
    FileResult result = FileIOErrorFromErrno(errno);
    if (result == FileResult::kInvalidArgument) {
      return FileResult::kNotFound;
    }
    return result;
  }
#endif

  // Finalize
  mIsOpen = true;
  return FileResult::kSuccess;
}

// -------------------------------------------------------------------------- //

FileResult
FileIO::Close()
{
  // Check that file is open
  if (!mIsOpen) {
    return FileResult::kNotOpen;
  }

  // Flush any content
  const FileResult result = Flush();
  if (result != FileResult::kSuccess) {
    return result;
  }

  // Close handle
#if defined(ALFLIB_TARGET_WINDOWS)
  CloseHandle(mFileHandle);
#else
  fclose(mFileHandle);
#endif

  // Finalize
  mIsOpen = false;
  return FileResult::kSuccess;
}

// -------------------------------------------------------------------------- //

FileResult
FileIO::Read(u8* buffer, u64 toRead, u64& read)
{
#if defined(ALFLIB_TARGET_WINDOWS)
  DWORD _read;
  const BOOL result =
    ReadFile(mFileHandle, buffer, static_cast<DWORD>(toRead), &_read, NULL);
  if (result != TRUE) {
    const DWORD error = GetLastError();
    return FileIOErrorFromErrorWin32(error);
  }
  read = static_cast<u64>(_read);
#else
  u64 _read = fread(buffer, 1, toRead, mFileHandle);
  if (_read != toRead) {
    if (feof(mFileHandle)) {
      return FileResult::kEOF;
    }
    return FileIOErrorFromErrno(ferror(mFileHandle));
  }
  read = _read;
#endif
  return FileResult::kSuccess;
}

// -------------------------------------------------------------------------- //

FileResult
FileIO::Read(String& string)
{
  const u64 size = mFile.GetSize();
  char8* buffer = DefaultAllocator::Instance().NewArray<char8>(size + 1);
  if (!buffer) {
    return FileResult::kOutOfMemory;
  }
  buffer[size] = 0;
  u64 read;
  const FileResult result = Read(reinterpret_cast<u8*>(buffer), size, read);
  if (result != FileResult::kSuccess) {
    DefaultAllocator::Instance().Delete(buffer);
    return result;
  }
  string = String(buffer);
  DefaultAllocator::Instance().Free(buffer);
  return FileResult::kSuccess;
}

// -------------------------------------------------------------------------- //

FileResult
FileIO::Write(const u8* buffer, u64 toWrite, u64& written) const
{
#if defined(ALFLIB_TARGET_WINDOWS)
  DWORD _written;
  const BOOL success = WriteFile(
    mFileHandle, buffer, static_cast<DWORD>(toWrite), &_written, NULL);
  if (success != TRUE) {
    return FileIOErrorFromErrorWin32(GetLastError());
  }
  written = _written;
#else
  u64 _written = fwrite(buffer, 1, toWrite, mFileHandle);
  if (_written != toWrite) {
    return FileIOErrorFromErrno(ferror(mFileHandle));
  }
  written = _written;
#endif
  return FileResult::kSuccess;
}

// -------------------------------------------------------------------------- //

FileResult
FileIO::Write(const String& string, u64& written) const
{
  return Write(
    reinterpret_cast<const u8*>(string.GetUTF8()), string.GetSize(), written);
}

// -------------------------------------------------------------------------- //

FileResult
FileIO::Flush() const
{
#if defined(ALFLIB_TARGET_WINDOWS)
  const BOOL success = FlushFileBuffers(mFileHandle);
  if (!success) {
    return FileIOErrorFromErrorWin32(GetLastError());
  }
#else
  fflush(mFileHandle);
#endif
  return FileResult::kSuccess;
}

// -------------------------------------------------------------------------- //

void
FileIO::Seek(u64 position) const
{
#if defined(ALFLIB_TARGET_WINDOWS)
  LARGE_INTEGER seekOffset;
  seekOffset.QuadPart = static_cast<LONGLONG>(position);
  SetFilePointerEx(mFileHandle, seekOffset, nullptr, FILE_BEGIN);
#else
  fseeko(mFileHandle, position, SEEK_CUR);
#endif
}

// -------------------------------------------------------------------------- //

void
FileIO::SeekEnd() const
{
#if defined(ALFLIB_TARGET_WINDOWS)
  LARGE_INTEGER seekOffset;
  seekOffset.QuadPart = 0ull;
  SetFilePointerEx(mFileHandle, seekOffset, nullptr, FILE_END);
#else
  fseeko(mFileHandle, 0, SEEK_END);
#endif
}

// -------------------------------------------------------------------------- //

u64
FileIO::GetCursorPosition() const
{
#if defined(ALFLIB_TARGET_WINDOWS)
  LARGE_INTEGER seekOffset;
  seekOffset.QuadPart = 0ull;
  return SetFilePointerEx(mFileHandle, seekOffset, nullptr, FILE_CURRENT);
#else
  return static_cast<u64>(ftell(mFileHandle));
#endif
}

}
