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

#include "alflib/memory/raw_memory_reader.hpp"

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

RawMemoryReader::RawMemoryReader(const u8* memory, u64 memorySize, u64 initialOffset)
  : mMemory(memory)
  , mMemorySize(memorySize)
  , mReadOffset(initialOffset)
{}

// -------------------------------------------------------------------------- //

const u8*
RawMemoryReader::ReadBytes(u64 size)
{
  AlfAssert(mReadOffset + size <= mMemorySize, "Reading past buffer");
  const u8* data = mMemory + mReadOffset;
  mReadOffset += size;
  return data;
}

// -------------------------------------------------------------------------- //

template<>
s8
RawMemoryReader::Read()
{
  const s8* bytes = reinterpret_cast<const s8*>(ReadBytes(sizeof(s8)));
  return *bytes;
}

// -------------------------------------------------------------------------- //

template<>
u8
RawMemoryReader::Read()
{
  const u8* bytes = ReadBytes(sizeof(s8));
  return *bytes;
}

// -------------------------------------------------------------------------- //

template<>
s16
RawMemoryReader::Read()
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
RawMemoryReader::Read()
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
RawMemoryReader::Read()
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
RawMemoryReader::Read()
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
RawMemoryReader::Read()
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
RawMemoryReader::Read()
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
RawMemoryReader::Read()
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
RawMemoryReader::Read()
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
RawMemoryReader::Read()
{
  u32 size = Read<u32>();
  const char8* data = reinterpret_cast<const char8*>(ReadBytes(size));
  return String(data, size);
}

}
