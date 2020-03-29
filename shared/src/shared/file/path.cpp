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

#include "path.hpp"

// ========================================================================== //
// Path Implementation
// ========================================================================== //

namespace wind {

Path::Path() {}

// -------------------------------------------------------------------------- //

Path::Path(const char8 *path) : m_str(path) { m_str.replace('\\', '/'); }

// -------------------------------------------------------------------------- //

Path::Path(const String &path) : m_str(path) { m_str.replace('\\', '/'); }

// -------------------------------------------------------------------------- //

Path::Path(const std::string &path) : m_str(String(path)) {
  m_str.replace('\\', '/');
}

// -------------------------------------------------------------------------- //

void Path::join(const Path &other) {
  m_str += "/";
  m_str += other.m_str;
}

// -------------------------------------------------------------------------- //

Path Path::joined(const Path &p0, const Path &p1) {
  Path out = p0;
  out.join(p1);
  return out;
}

// -------------------------------------------------------------------------- //

Path Path::parent() const {
  const s64 idx = m_str.findLast('/');
  if (idx != -1) {
    return m_str.substring(0, idx);
  }
  return ".";
}

// -------------------------------------------------------------------------- //

String Path::getName() const {
  const s64 idx = m_str.findLast('/');
  return m_str.substring(idx + 1, String::kEntireStr);
}

// -------------------------------------------------------------------------- //

String Path::getBaseName() const {
  const s64 sepIndex = m_str.findLast('/');
  const s64 dotIndex = m_str.findLast('.');

  if (dotIndex <= sepIndex) {
    return m_str.substring(sepIndex + 1, String::kEntireStr);
  }
  return m_str.substring(sepIndex + 1, dotIndex - sepIndex - 1);
}

// -------------------------------------------------------------------------- //

String Path::getExtString() const {
  const s64 idx = m_str.findLast('.');
  if (idx != -1) {
    return m_str.substring(idx + 1, String::kEntireStr);
  }
  return "";
}

// -------------------------------------------------------------------------- //

Path::Ext Path::getExt() const {
  const String ext = getExtString();
  if (ext == "") {
    return Ext::kNone;
  }
  if (ext == ".txt") {
    return Ext::kTxt;
  }
  if (ext == ".png") {
    return Ext::kPng;
  }
  if (ext == ".hlsl") {
    return Ext::kHlsl;
  }
  return Ext::kUnknown;
}

} // namespace wind
