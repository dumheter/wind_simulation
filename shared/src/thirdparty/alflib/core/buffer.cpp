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

#include "alflib/core/buffer.hpp"

// ========================================================================== //
// Buffer Implementation
// ========================================================================== //

namespace alflib {

Buffer::Buffer(u64 size, const u8* data, Allocator& allocator)
  : mAllocator(allocator)
  , mSize(size)
{
  AlfAssert(size > 0, "Buffer size must not be zero (0)");
  mData = mAllocator.NewArray<u8>(size);
  if (data) {
    memcpy(mData, data, size);
  }
}

// -------------------------------------------------------------------------- //

Buffer::Buffer(const Buffer& other)
  : mAllocator(other.mAllocator)
  , mData(nullptr)
  , mSize(other.mSize)
{
  if (other.mData) {
    mData = mAllocator.NewArray<u8>(mSize);
    memcpy(mData, other.mData, mSize);
  }
}

// -------------------------------------------------------------------------- //

Buffer::Buffer(Buffer&& other)
  : mAllocator(other.mAllocator)
  , mData(other.mData)
  , mSize(other.mSize)
{
  other.mData = nullptr;
  other.mSize = 0;
}

// -------------------------------------------------------------------------- //

Buffer::~Buffer()
{
  mAllocator.Free(mData);
  mSize = 0;
}

// -------------------------------------------------------------------------- //
Buffer&
Buffer::operator=(const Buffer& other)
{
  if (this != &other) {
    mAllocator.SafeFree(reinterpret_cast<void**>(&mData));
    mAllocator = other.mAllocator;
    mSize = other.mSize;

    if (other.mData) {
      mData = mAllocator.NewArray<u8>(mSize);
      memcpy(mData, other.mData, mSize);
    }
  }
  return *this;
}

// -------------------------------------------------------------------------- //

Buffer&
Buffer::operator=(Buffer&& other)
{
  if (this != &other) {
    mAllocator.Free(mData);

    mAllocator = other.mAllocator;
    mData = other.mData;
    mSize = other.mSize;
    other.mData = nullptr;
    other.mSize = 0;
  }
  return *this;
}

// -------------------------------------------------------------------------- //

void
Buffer::Write(u64 offset, const u8* data, u64 size)
{
  // Assert precondition
  AlfAssert(offset + size <= mSize,
            "Offset + size is outside the bounds of the buffer {} > {}",
            offset + size,
            mSize);

  // Write if the buffer is valid
  if (mData) {
    memcpy(mData + offset, data, size);
  }
}

// -------------------------------------------------------------------------- //

void
Buffer::Resize(u64 size, bool retainData)
{
  // Resize buffer
  if (retainData && mData) {
    u8* buffer = mAllocator.NewArray<u8>(size);
    memcpy(buffer, mData, Min(size, mSize));
    mAllocator.Free(mData);
    mData = buffer;
    mSize = size;
  } else {
    mAllocator.Free(mData);
    mSize = size;
    mData = mAllocator.NewArray<u8>(mSize);
  }
}

// -------------------------------------------------------------------------- //

void
Buffer::Clear()
{
  if (mData) {
    memset(mData, 0, mSize);
  }
}

// -------------------------------------------------------------------------- //

Buffer
Buffer::CopyRegion(u64 offset, u64 size)
{
  // Assert precondition
  AlfAssert(offset + size <= mSize,
            "Offset + size is outside the bounds of the buffer {} > {}",
            offset + size,
            mSize);

  // Copy region
  if (!mData) {
    return Buffer(size, nullptr, mAllocator);
  }
  return Buffer(size, mData + offset, mAllocator);
}

// -------------------------------------------------------------------------- //

u8*
Buffer::TakeData()
{
  u8* data = mData;
  mData = nullptr;
  mSize = 0;
  return data;
}

}