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

#include "alflib/memory/raw_memory_writer.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Project headers
#include "alflib/memory/memory.hpp"

// ========================================================================== //
// MemoryWriter Implementation
// ========================================================================== //

namespace alflib {

RawMemoryWriter::RawMemoryWriter(u8 *memory, u64 memorySize, u64 initialOffset)
    : mMemory(memory), mMemorySize(memorySize), mWriteOffset(initialOffset) {}

// -------------------------------------------------------------------------- //

bool RawMemoryWriter::WriteBytes(const u8 *data, u64 size) {
  // Resize buffer if neccessary
  if (mMemorySize < mWriteOffset + size) {
    return false;
  }

  // Write bytes
  memcpy(mMemory + mWriteOffset, data, size);
  mWriteOffset += size;
  return true;
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const s8 &object) {
  return Write(reinterpret_cast<const u8 &>(object));
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const u8 &object) {
  return WriteBytes(&object, 1);
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const s16 &object) {
  return Write(reinterpret_cast<const u16 &>(object));
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const u16 &object) {
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return WriteBytes(reinterpret_cast<const u8 *>(&object), sizeof(u16));
#else
  u16 _object = SwapEndian(object);
  return WriteBytes(reinterpret_cast<const u8 *>(&_object), sizeof(u16));
#endif
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const s32 &object) {
  return Write(reinterpret_cast<const u32 &>(object));
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const u32 &object) {
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return WriteBytes(reinterpret_cast<const u8 *>(&object), sizeof(u32));
#else
  u32 _object = SwapEndian(object);
  return WriteBytes(reinterpret_cast<const u8 *>(&_object), sizeof(u32));
#endif
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const s64 &object) {
  return Write(reinterpret_cast<const u64 &>(object));
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const u64 &object) {
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return WriteBytes(reinterpret_cast<const u8 *>(&object), sizeof(u64));
#else
  u64 _object = SwapEndian(object);
  return WriteBytes(reinterpret_cast<const u8 *>(&_object), sizeof(u64));
#endif
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const f32 &object) {
  return Write(reinterpret_cast<const u32 &>(object));
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const f64 &object) {
  return Write(reinterpret_cast<const u64 &>(object));
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const String &object) {
  u32 size = object.GetSize();
  bool success = Write(size);
  if (!success) {
    return false;
  }
  return WriteBytes(reinterpret_cast<const u8 *>(object.GetUTF8()), size);
}

// -------------------------------------------------------------------------- //

template <> bool RawMemoryWriter::Write(const char8 *object) {
  u32 size = u32(strlen(object));
  bool success = Write(size);
  if (!success) {
    return false;
  }
  return WriteBytes(reinterpret_cast<const u8 *>(object), size);
}

} // namespace alflib