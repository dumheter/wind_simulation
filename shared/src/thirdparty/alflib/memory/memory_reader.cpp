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

#include "alflib/memory/memory_reader.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// Project headers
#include "alflib/memory/memory.hpp"
#include "alflib/core/assert.hpp"

// ========================================================================== //
// MemoryReader Implementation
// ========================================================================== //

namespace alflib {

MemoryReader::MemoryReader(const Buffer& buffer, u64 initialOffset)
  : mBuffer(buffer)
  , mReadOffset(initialOffset)
{}

// -------------------------------------------------------------------------- //

const u8*
MemoryReader::ReadBytes(u64 size)
{
  AlfAssert(mReadOffset + size <= mBuffer.GetSize(), "Reading past buffer");
  const u8* data = mBuffer.GetData() + mReadOffset;
  mReadOffset += size;
  return data;
}

// -------------------------------------------------------------------------- //

template<>
s8
MemoryReader::Read()
{
  const s8* bytes = reinterpret_cast<const s8*>(ReadBytes(sizeof(s8)));
  return *bytes;
}

// -------------------------------------------------------------------------- //

template<>
u8
MemoryReader::Read()
{
  const u8* bytes = ReadBytes(sizeof(s8));
  return *bytes;
}

// -------------------------------------------------------------------------- //

template<>
s16
MemoryReader::Read()
{
  const s16* data = reinterpret_cast<const s16*>(ReadBytes(sizeof(s16)));
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return *data;
#else
  return SwapEndian(*data);
#endif
}

// -------------------------------------------------------------------------- //

template<>
u16
MemoryReader::Read()
{
  const u16* data = reinterpret_cast<const u16*>(ReadBytes(sizeof(u16)));
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return *data;
#else
  return SwapEndian(*data);
#endif
}

// -------------------------------------------------------------------------- //

template<>
s32
MemoryReader::Read()
{
  const s32* data = reinterpret_cast<const s32*>(ReadBytes(sizeof(s32)));
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return *data;
#else
  return SwapEndian(*data);
#endif
}

// -------------------------------------------------------------------------- //

template<>
u32
MemoryReader::Read()
{
  const u32* data = reinterpret_cast<const u32*>(ReadBytes(sizeof(u32)));
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return *data;
#else
  return SwapEndian(*data);
#endif
}

// -------------------------------------------------------------------------- //

template<>
s64
MemoryReader::Read()
{
  const u64* data = reinterpret_cast<const u64*>(ReadBytes(sizeof(s64)));
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return *data;
#else
  return SwapEndian(*data);
#endif
}

// -------------------------------------------------------------------------- //

template<>
u64
MemoryReader::Read()
{
  const u64* data = reinterpret_cast<const u64*>(ReadBytes(sizeof(u64)));
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return *data;
#else
  return SwapEndian(*data);
#endif
}

// -------------------------------------------------------------------------- //

template<>
f32
MemoryReader::Read()
{
  const f32* data = reinterpret_cast<const f32*>(ReadBytes(sizeof(f32)));
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return *data;
#else
  return SwapEndian(*data);
#endif
}

// -------------------------------------------------------------------------- //

template<>
f64
MemoryReader::Read()
{
  const f64* data = reinterpret_cast<const f64*>(ReadBytes(sizeof(f64)));
#if ALFLIB_HOST_ENDIAN == ALFLIB_LITTLE_ENDIAN
  return *data;
#else
  return SwapEndian(*data);
#endif
}

// -------------------------------------------------------------------------- //

template<>
String
MemoryReader::Read()
{
  u32 size = Read<u32>();
  const char8* data = reinterpret_cast<const char8*>(ReadBytes(size));
  return String(data, size);
}

}