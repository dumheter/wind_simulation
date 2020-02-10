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

#include "alflib/file/path.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Standard headers
#include <cstdlib>

// Project headers
#include <alflib/platform/os.hpp>
#include <alflib/platform/platform.hpp>

// ========================================================================== //
// Path Implementation
// ========================================================================== //

namespace alflib {

const String Path::CURRENT = ".";

// -------------------------------------------------------------------------- //

const String Path::PARENT = "..";

// -------------------------------------------------------------------------- //

#if defined(ALFLIB_TARGET_WINDOWS)
const String Path::SEPARATOR = "\\";
#else
const String Path::SEPARATOR = "/";
#endif

// -------------------------------------------------------------------------- //

Path::Path(const String &path) : mPath(path) {
  // Replace separators with native ones
#if defined(ALFLIB_TARGET_WINDOWS)
  mPath.Replace("/", "\\");
#else
  mPath.Replace("\\", "/");
#endif

  // Remove trailing separator
  if (mPath.EndsWith('\\') || mPath.EndsWith('/')) {
    mPath = mPath.Substring(0, mPath.GetLength() - 1);
  }
}

// -------------------------------------------------------------------------- //

Path &Path::Join(const Path &other) {
  // Check if a separator should be added
  if (!other.mPath.StartsWith('\\') && !other.mPath.StartsWith('/') &&
      mPath.GetLength() > 0) {
    mPath += SEPARATOR_CHAR;
  }
  mPath += other.mPath;
  return *this;
}

// -------------------------------------------------------------------------- //

Path &Path::operator+=(const Path &other) { return Join(other); }

// -------------------------------------------------------------------------- //

Path Path::Joined(const Path &other) const {
  Path path = *this;
  path.Join(other);
  return path;
}

// -------------------------------------------------------------------------- //

Path Path::GetAbsolutePath() const {
#if defined(ALFLIB_TARGET_WINDOWS)
  // Retrieve absolute path
  char16 buffer[MAX_PATH];
  const DWORD length =
      GetFullPathNameW(mPath.GetUTF16().Get(), MAX_PATH, buffer, nullptr);
  if (length != 0) {
    // Canonicalize path
    char16 _buffer[MAX_PATH];
    // TODO(Filip Björklund): Handle case where function is not available
    const HRESULT result =
        SharedLibraries::GetKernelBase().pPathCchCanonicalizeEx(
            _buffer, MAX_PATH, buffer, PATHCCH_NONE);
    if (SUCCEEDED(result)) {
      return String(_buffer);
    }
  }
  return Path{""};
#else
  char buffer[PATH_MAX];
  realpath(mPath.GetUTF8(), buffer);
  return String(buffer);
#endif
}

// -------------------------------------------------------------------------- //

Path Path::GetCanonical() const {
  // Empty path or single '.' is empty
  if (mPath.GetLength() == 0 || mPath == ".") {
    return Path{""};
  }

  // Retrieve components
  ArrayList<String> components = GetComponents();

  // Build path
  Path path;
  const u32 componentCount = u32(components.GetSize());
  for (u32 i = 0; i < componentCount; ++i) {
    // Remove any (non '..') component before a '..'
    if (components[i] != ".." && componentCount > i + 1 &&
        components[i + 1] == "..") {
      i++;
      continue;
    }

    // 4. Ignore dots
    if (components[i] == ".") {
      continue;
    }

    // Concatenate
    path.Join(components[i]);
  }

  return path;
}

// -------------------------------------------------------------------------- //

Path Path::GetDirectory() const {
  const s64 sepIndex = Max(mPath.LastIndexOf('/'), mPath.LastIndexOf('\\'));
  if (sepIndex < 0) {
    return Path{""};
  }
  return Path{mPath.Substring(0, sepIndex)};
}

// -------------------------------------------------------------------------- //

ArrayList<String> Path::GetComponents() const {
  ArrayList<String> components;

  // Find components
  u32 prevIndex = 0;
  mPath.ForEach([&](u32 codepoint, u32 index) {
    if (codepoint == '/' || codepoint == '\\') {
      String substring = mPath.Substring(prevIndex, index - prevIndex);
      if (substring.GetSize() > 0) {
        components.Append(substring);
      }
      prevIndex = index + 1;
    }
  });
  components.Append(mPath.Substring(prevIndex));
  return components;
}

// -------------------------------------------------------------------------- //

String Path::GetName() const {
  // If index is not valid (-1), then it becomes 0 in the same way that the
  // separator are skipped (+1).
  const s64 sepIndex = Max(mPath.LastIndexOf('/'), mPath.LastIndexOf('\\'));
  return mPath.Substring(sepIndex + 1);
}

// -------------------------------------------------------------------------- //

String Path::GetBaseName() const {
  // Retrieve indices of last separator and 'dot'
  const s64 sepIndex = Max(mPath.LastIndexOf('/'), mPath.LastIndexOf('\\'));
  const s64 dotIndex = mPath.LastIndexOf('.');

  if (dotIndex <= sepIndex) {
    return mPath.Substring(sepIndex + 1);
  }
  return mPath.Substring(sepIndex + 1, dotIndex - sepIndex - 1);
}

// -------------------------------------------------------------------------- //

Path::Extension Path::GetExtension() const {
  const String extensionString = GetExtensionString();
  if (extensionString.GetLength() == 0) {
    return Extension::kNone;
  }

  // Data (text, binary, config, ...)
  if (extensionString == ".tmp") {
    return Extension::kTmp;
  }
  if (extensionString == ".txt") {
    return Extension::kTxt;
  }
  if (extensionString == ".csv") {
    return Extension::kCsv;
  }
  if (extensionString == ".dat") {
    return Extension::kDat;
  }
  if (extensionString == ".json") {
    return Extension::kJson;
  }
  if (extensionString == ".xml") {
    return Extension::kXml;
  }
  if (extensionString == ".yaml") {
    return Extension::kYaml;
  }
  if (extensionString == ".toml") {
    return Extension::kToml;
  }
  if (extensionString == ".md") {
    return Extension::kMd;
  }
  if (extensionString == ".cfg") {
    return Extension::kCfg;
  }
  if (extensionString == ".ini") {
    return Extension::kIni;
  }
  if (extensionString == ".log") {
    return Extension::kLog;
  }

  // Image
  if (extensionString == ".png") {
    return Extension::kPng;
  }
  if (extensionString == ".tga") {
    return Extension::kTga;
  }
  if (extensionString == ".jpeg" || extensionString == ".jpg") {
    return Extension::kJpeg;
  }
  if (extensionString == ".psd") {
    return Extension::kPsd;
  }
  if (extensionString == ".bmp") {
    return Extension::kBmp;
  }
  if (extensionString == ".gif") {
    return Extension::kGif;
  }
  if (extensionString == ".ico") {
    return Extension::kIco;
  }
  if (extensionString == ".svg") {
    return Extension::kSvg;
  }
  if (extensionString == ".tiff" || extensionString == ".tif") {
    return Extension::kTiff;
  }

  // Audio
  if (extensionString == ".ogg") {
    return Extension::kOgg;
  }
  if (extensionString == ".wav") {
    return Extension::kWav;
  }
  if (extensionString == ".mp3") {
    return Extension::kMp3;
  }

  // Video
  if (extensionString == ".avi") {
    return Extension::kAvi;
  }
  if (extensionString == ".mp4") {
    return Extension::kMp4;
  }

  // Archive
  if (extensionString == ".tar") {
    return Extension::kTar;
  }
  if (extensionString == ".zip") {
    return Extension::kZip;
  }
  if (extensionString == ".gz") {
    return Extension::kGz;
  }
  if (extensionString == ".7z") {
    return Extension::k7z;
  }

  // Font
  if (extensionString == ".ttf") {
    return Extension::kTtf;
  }
  if (extensionString == ".otf") {
    return Extension::kOtf;
  }

  // Code (source, headers)
  if (extensionString == ".c") {
    return Extension::kC;
  }
  if (extensionString == ".h") {
    return Extension::kH;
  }
  if (extensionString == ".cpp") {
    return Extension::kCpp;
  }
  if (extensionString == ".hpp") {
    return Extension::kHpp;
  }
  if (extensionString == ".py") {
    return Extension::kPy;
  }
  if (extensionString == ".js") {
    return Extension::kJs;
  }
  if (extensionString == ".java") {
    return Extension::kJava;
  }
  if (extensionString == ".rs") {
    return Extension::kRs;
  }

  // Executables, libraries, ...
  if (extensionString == ".exe") {
    return Extension::kExe;
  }
  if (extensionString == ".app") {
    return Extension::kApp;
  }
  if (extensionString == ".apk") {
    return Extension::kApk;
  }
  if (extensionString == ".dll") {
    return Extension::kDll;
  }
  if (extensionString == ".so") {
    return Extension::kSo;
  }
  if (extensionString == ".dynlib") {
    return Extension::kDynlib;
  }
  if (extensionString == ".lib") {
    return Extension::kLib;
  }
  if (extensionString == ".a") {
    return Extension::kA;
  }

  // Could not determine
  return Extension::kUnknown;
}

// -------------------------------------------------------------------------- //

String Path::GetExtensionString() const {
  const s64 sepIndex = Max(mPath.LastIndexOf('/'), mPath.LastIndexOf('\\'));
  const s64 dotIndex = mPath.LastIndexOf('.');
  if (dotIndex <= sepIndex) {
    return "";
  }
  const String extension = mPath.Substring(dotIndex);
  return extension.GetLength() > 1 ? extension : "";
}

// -------------------------------------------------------------------------- //

bool operator==(const Path &path0, const Path &path1) {
  return path0.GetCanonical().GetPathString() ==
         path1.GetCanonical().GetPathString();
}

// -------------------------------------------------------------------------- //

bool operator!=(const Path &path0, const Path &path1) {
  return path0.GetCanonical().GetCanonical() ==
         path1.GetCanonical().GetPathString();
}

// -------------------------------------------------------------------------- //

Path operator+(const Path &path0, const Path &path1) {
  return path0.Joined(path1);
}

// -------------------------------------------------------------------------- //

Path operator+(const Path &path0, const String &path1) {
  return path0.Joined(Path{path1});
}

// -------------------------------------------------------------------------- //

Path Path::GetKnownDirectory(KnownDirectory directory) {
#if defined(ALFLIB_TARGET_WINDOWS)
  if (directory == KnownDirectory::kHome) {
    char16 *buffer;
    HRESULT hresult = SHGetKnownFolderPath(FOLDERID_Profile, KF_FLAG_DEFAULT,
                                           nullptr, &buffer);
    String output(buffer);
    CoTaskMemFree(buffer);
    return output;
  }
  if (directory == KnownDirectory::kDesktop) {
    char16 *buffer;
    HRESULT hresult = SHGetKnownFolderPath(FOLDERID_Desktop, KF_FLAG_DEFAULT,
                                           nullptr, &buffer);
    String output(buffer);
    CoTaskMemFree(buffer);
    return output;
  }
  if (directory == KnownDirectory::kDocuments) {
    char16 *buffer;
    HRESULT hresult = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT,
                                           nullptr, &buffer);
    String output(buffer);
    CoTaskMemFree(buffer);
    return output;
  }
  if (directory == KnownDirectory::kDownloads) {
    char16 *buffer;
    HRESULT hresult = SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_DEFAULT,
                                           nullptr, &buffer);
    String output(buffer);
    CoTaskMemFree(buffer);
    return output;
  }
#else
  if (directory == KnownDirectory::kHome) {
    struct passwd *pw = getpwuid(getuid());
    return String{pw->pw_dir};
  }
  if (directory == KnownDirectory::kDesktop) {
    String dir = Linux::GetXDGDirectories().desktop;
    if (dir.GetLength() == 0) {
      return Path{""};
    }
    return Path{dir};
  }
  if (directory == KnownDirectory::kDocuments) {
    String dir = Linux::GetXDGDirectories().documents;
    if (dir.GetLength() == 0) {
      return Path{""};
    }
    return Path{dir};
  }
  if (directory == KnownDirectory::kDownloads) {
    String dir = Linux::GetXDGDirectories().download;
    if (dir.GetLength() == 0) {
      return Path{""};
    }
    return Path{dir};
  }
#endif
  return Path{""};
}

} // namespace alflib
