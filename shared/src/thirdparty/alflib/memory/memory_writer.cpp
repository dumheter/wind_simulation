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

#include "alflib/memory/memory_writer.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Project headers
#include "alflib/memory/memory.hpp"

// ========================================================================== //
// MemoryWriter Implementation
// ========================================================================== //

namespace alflib {

MemoryWriter::MemoryWriter(Buffer &buffer, u64 initialOffset)
    : mBuffer(buffer), mWriteOffset(initialOffset) {}

// -------------------------------------------------------------------------- //

void MemoryWriter::WriteBytes(const u8 *data, u64 size) {
  // Resize buffer if neccessary
  if (mBuffer.GetSize() < mWriteOffset + size) {
    mBuffer.Resize(u64(mBuffer.GetSize() * BUFFER_RESIZE_FACTOR));
  }

  // Write bytes
  mBuffer.Write(mWriteOffset, data, size);
  mWriteOffset += size;
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const s8 &object) {
  Write(reinterpret_cast<const u8 &>(object));
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const u8 &object) {
  WriteBytes(&object, 1);
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const s16 &object) {
  Write(reinterpret_cast<const u16 &>(object));
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const u16 &object) {
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  WriteBytes(reinterpret_cast<const u8 *>(&object), sizeof(u16));
#else
  u16 _object = SwapEndian(object);
  WriteBytes(reinterpret_cast<const u8 *>(&_object), sizeof(u16));
#endif
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const s32 &object) {
  Write(reinterpret_cast<const u32 &>(object));
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const u32 &object) {
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  WriteBytes(reinterpret_cast<const u8 *>(&object), sizeof(u32));
#else
  u32 _object = SwapEndian(object);
  WriteBytes(reinterpret_cast<const u8 *>(&_object), sizeof(u32));
#endif
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const s64 &object) {
  Write(reinterpret_cast<const u64 &>(object));
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const u64 &object) {
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  WriteBytes(reinterpret_cast<const u8 *>(&object), sizeof(u64));
#else
  u64 _object = SwapEndian(object);
  WriteBytes(reinterpret_cast<const u8 *>(&_object), sizeof(u64));
#endif
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const f32 &object) {
  Write(reinterpret_cast<const u32 &>(object));
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const f64 &object) {
  Write(reinterpret_cast<const u64 &>(object));
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const String &object) {
  u32 size = object.GetSize();
  Write(size);
  WriteBytes(reinterpret_cast<const u8 *>(object.GetUTF8()), size);
}

// -------------------------------------------------------------------------- //

template <> void MemoryWriter::Write(const char8 *object) {
  u32 size = u32(strlen(object));
  Write(size);
  WriteBytes(reinterpret_cast<const u8 *>(object), size);
}

} // namespace alflib