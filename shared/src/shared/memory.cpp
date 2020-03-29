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

#include "memory.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <cstring>
#include <new>

#include <mimalloc.h>

// ========================================================================== //
// New/Delete
// ========================================================================== //

void *operator new(std::size_t n) { return wind::Memory::alloc(n); }

// -------------------------------------------------------------------------- //

void operator delete(void *p) { wind::Memory::free(p); }

// ========================================================================== //
// Memory Implementation
// ========================================================================== //

namespace wind {

void *Memory::alloc(u64 size, u64 align) {
  return mi_malloc_aligned(size, align);
}

// -------------------------------------------------------------------------- //

void *Memory::realloc(void *mem, u64 size, u64 align) {
  return mi_realloc_aligned(mem, size, align);
}

// -------------------------------------------------------------------------- //

void Memory::free(void *mem) { mi_free(mem); }

// -------------------------------------------------------------------------- //

void *Memory::copy(void *dst, void const *src, u64 size) {
  return memcpy(dst, src, size);
}

} // namespace wind
