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

#include "string.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <plommon/pm_unicode.h>

#include "shared/assert.hpp"
#include "shared/memory.hpp"

// ========================================================================== //
// String Implementation
// ========================================================================== //

namespace wind {

String::String() {}

// -------------------------------------------------------------------------- //

String::String(const char8 *str) : m_buf(str) {}

// -------------------------------------------------------------------------- //

String::String(const std::string &str) : m_buf(str) {}

// -------------------------------------------------------------------------- //

s64 String::find(u32 cp) const {
  return pmOffOf8(m_buf.c_str(), m_buf.size(), cp);
}

// -------------------------------------------------------------------------- //

s64 String::find(const String &substr) const {
  const auto pos = m_buf.find(substr.m_buf);
  if (pos != std::string::npos) {
    return s64(pos);
  }
  return -1;
}

// -------------------------------------------------------------------------- //

s64 String::findLast(u32 cp) const {
  return pmLastOffOf8(m_buf.c_str(), m_buf.size(), cp);
}

// -------------------------------------------------------------------------- //

String String::substring(u64 offset, u64 count) const {
  if (count == kEntireStr) {
    count = getSize() - offset;
  }
  pm_char8 *buf = pmSubstr8(getUTF8(), offset, count, Memory::alloc);
  String out(buf);
  Memory::free(buf);
  return out;
}

// -------------------------------------------------------------------------- //

void String::replace(Codepoint from, Codepoint to) {
  /// to == from
  if (unitCount(to) == unitCount(from)) {
    pm_char8 *out =
        pmReplace8(m_buf.data(), m_buf.size(), from, to, Memory::alloc);
    WN_ASSERT(out == m_buf.data(), "Output is expected to not be allocated");
    return;
  }

  /// to < from
  if (unitCount(to) < unitCount(from)) {
    pm_char8 *in = copyCStr(getUTF8());
    pm_char8 *out = pmReplace8(in, strlen(in), from, to, Memory::alloc);
    WN_ASSERT(in == out, "Output is expected to not be allocated");
    m_buf = in;
    Memory::free(in);
    return;
  }

  /// to > from
  pm_char8 *out =
      pmReplace8(m_buf.data(), m_buf.size(), from, to, Memory::alloc);
  WN_ASSERT(out != m_buf.data(), "Output is expected to be allocated");
  m_buf = out;
  Memory::free(out);
}

// -------------------------------------------------------------------------- //

u32 String::unitCount(Codepoint cp) { return pmUnitCount8(cp); }

// -------------------------------------------------------------------------- //

char8 *String::copyCStr(const char8 *str) {
  const size_t size = strlen(str);
  char8 *buf = static_cast<char8 *>(Memory::alloc(sizeof(char8) * (size + 1)));
  return static_cast<char8 *>(Memory::copy(buf, str, size + 1));
}

// -------------------------------------------------------------------------- //

void String::append(const String &other) { m_buf += other.m_buf; }

// -------------------------------------------------------------------------- //

String operator+(const String &s0, const String &s1) {
  return String(s0.m_buf + s1.m_buf);
}

// -------------------------------------------------------------------------- //

bool operator==(const String &s0, const String &s1) {
  return s0.m_buf == s1.m_buf;
}

// -------------------------------------------------------------------------- //

bool operator!=(const String &s0, const String &s1) { return !(s0 == s1); }

} // namespace wind
