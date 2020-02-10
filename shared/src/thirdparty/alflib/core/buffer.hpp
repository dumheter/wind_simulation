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

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

// Project headers
#include "alflib/core/macros.hpp"
#include "alflib/memory/allocator.hpp"
#include "alflib/collection/array_list.hpp"

// ========================================================================== //
// Buffer Declaration
// ========================================================================== //

namespace alflib {

/** \class Buffer
 * \author Filip Björklund
 * \date 01 januari 2019 - 00:00
 * \brief Memory buffer.
 * \details
 * A buffer of 'u8' data. This can be used to hold any type of binary, or other,
 * data.
 */
class Buffer
{
private:
  /** Allocator **/
  Allocator& mAllocator;

  /** Buffer data **/
  u8* mData = nullptr;
  /** Buffer size **/
  u64 mSize = 0;

public:
  /** Construct a buffer with a specified size. Optionally data and an allocator
   * can be specified.
   * \note The data is expected to be sized according to the size of the buffer.
   * \pre Size must be greater than zero.
   * \brief Construct buffer with size and optional data.
   * \param size Size of the buffer.
   * \param data Data to fill buffer with.
   * \param allocator Allocator to allocate buffer with.
   */
  explicit Buffer(u64 size,
                  const u8* data = nullptr,
                  Allocator& allocator = DefaultAllocator::Instance());

  /** Copy-constructor **/
  Buffer(const Buffer& other);

  /** Move-constructor **/
  Buffer(Buffer&& other);

  /** Destruct buffer **/
  ~Buffer();

  /** Copy-assigmnent **/
  Buffer& operator=(const Buffer& other);

  /** Move-assigmnent **/
  Buffer& operator=(Buffer&& other);

  /** Write data to the buffer at the specified buffer.
   * \brief Write data to buffer.
   * \param offset Offset to write data at.
   * \param data Data to write.
   * \param size Size of data to write.
   */
  void Write(u64 offset, const u8* data, u64 size);

  /** Resize the buffer to a new size.
   * \brief Resize buffer.
   * \param size Size to resize buffer to.
   * \param retainData Whether the data in the buffer should be retained after
   * resize.
   */
  void Resize(u64 size, bool retainData = true);

  /** Clear the content of the buffer to all zero.
   * \brief Clear buffer.
   */
  void Clear();

  /** Copy a region of the buffer into a new buffer.
   * \brief Copy region.
   * \param offset Offset to begin copy at.
   * \param size Size of region to copy.
   * \return Buffer representing the copied region.
   */
  Buffer CopyRegion(u64 offset, u64 size);

  /** Take ownership of the buffer data. The buffer will have data that is null
   * after this call. However data can be allocated for it by calling resize on
   * it.
   * \brief Take buffer data.
   * \return Buffer data.
   */
  u8* TakeData();

  /** Returns the raw underlying data of the buffer.
   * \brief Returns raw data.
   * \return Raw data.
   */
  u8* GetData() { return mData; }

  /** Returns the raw underlying data of the buffer.
   * \brief Returns raw data.
   * \return Raw data.
   */
  const u8* GetData() const { return mData; }

  /** Returns the size of the buffer.
   * \brief Returns buffer size.
   * \return Buffer size in bytes.
   */
  ArrayList<u8>::SizeType GetSize() const { return mSize; }
};

}