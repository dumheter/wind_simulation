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

// Standard headers
#include <unordered_map>
#include <vector>

// Project headers
#include "alflib/core/buffer.hpp"

// ========================================================================== //
// MemoryWriter Declaration
// ========================================================================== //

namespace alflib {

/** \class RawMemoryWriter.
 * \author Filip Björklund
 * \date 28 june 2019 - 10:06
 * \brief Raw memory writer.
 * \details
 * Class for writing binary data to a memory buffer.
 *
 * It should be noted that the contents of the buffer is stored in little-endian
 * byte order. This means that little-endian machines can write without
 * conversion.
 */

class RawMemoryWriter
{
public:
  /** Buffer resize factor **/
  static constexpr f32 BUFFER_RESIZE_FACTOR = 2.0f;
  /** Default buffer size **/
  static constexpr u64 DEFAULT_BUFFER_SIZE = 16;

private:
  /** Memory that is being written to **/
  u8* mMemory;
  /** Size of the memory allocation **/
  u64 mMemorySize;
  /** Write offset **/
  u64 mWriteOffset = 0;

public:
  /** Construct a memory writer from a memory pointer, size and an optional
   * initial offset.
   * \brief Construct memory writer.
   * \param memory Memory to write to.
   * \param memorySize Size of the memory allocation.
   * \param initialOffset Initial offset to write at in memory.
   */
  explicit RawMemoryWriter(u8* memory, u64 memorySize, u64 initialOffset = 0);

  /** Write raw bytes to the memory buffer.
   * \brief Write bytes.
   * \param data Bytes to write.
   * \param size Size of data to write.
   * \return True if the bytes could be written else false.
   */
  bool WriteBytes(const u8* data, u64 size);

  /** Write an object of the type T to buffer.
   * \note This function requires the object to have a function
   * 'ToBytes(MemoryWriter& writer)' defined for its class.
   * \brief Write object.
   * \tparam T Type of object to write.
   * \param object Object to write.
   * \return True if the bytes could be written else false.
   */
  template<typename T>
  bool Write(const T& object);

  /** Write an object of the type T to buffer.
   * \note This function requires the object to have a function
   * 'ToBytes(MemoryWriter& writer)' defined for its class.
   * \brief Write object.
   * \tparam T Type of object to write.
   * \param object Object to write.
   * \return True if the bytes could be written else false.
   */
  template<typename T>
  bool Write(const T* object);

  /** Write a list of objects to the buffer.
   * \brief Write list.
   * \tparam T Type of objects in list.
   * \param list List to write.
   * \return True if the bytes could be written else false.
   */
  template<typename T>
  bool Write(const ArrayList<T>& list);

  /** Write a std::vector to the buffer.
   * \brief Write vector.
   * \tparam T Object type.
   * \param vector Vector to write.
   * \return True if the bytes could be written else false.
   */
  template<typename T>
  bool Write(const std::vector<T>& vector);

  /** Write a std::unordered_map to the buffer.
   * \brief Write map.
   * \tparam K Key type.
   * \tparam V Value type.
   * \param map Map to write.
   * \return True if the bytes could be written else false.
   */
  template<typename K, typename V>
  bool Write(const std::unordered_map<K, V>& map);

  /** Returns the current write offset in the underlying buffer.
   * \brief Returns write offset.
   * \return Write offset.
   */
  u64 GetOffset() const { return mWriteOffset; }

  /** Set the write offset of the memory writer.
   * \brief Set write offset.
   * \param offset Write offset to set.
   */
  void SetOffset(u64 offset) { mWriteOffset = offset; }
};

// -------------------------------------------------------------------------- //

template<typename T>
bool
RawMemoryWriter::Write(const T& object)
{
  return object.ToBytes(*this);
}

// -------------------------------------------------------------------------- //

template<typename T>
bool
RawMemoryWriter::Write(const T* object)
{
  return object->ToBytes(*this);
}

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const s8& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const u8& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const s16& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const u16& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const s32& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const u32& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const s64& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const u64& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const f32& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const f64& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const String& object);

// -------------------------------------------------------------------------- //

template<>
bool
RawMemoryWriter::Write(const char8* object);

// -------------------------------------------------------------------------- //

template<typename T>
bool
RawMemoryWriter::Write(const ArrayList<T>& list)
{
  bool success = Write(static_cast<u64>(list.GetSize()));
  if (!success) {
    return false;
  }
  for (const T& object : list) {
    success = Write(object);
    if (!success) {
      return false;
    }
  }
}

// -------------------------------------------------------------------------- //

template<typename T>
bool
RawMemoryWriter::Write(const std::vector<T>& vector)
{
  bool success = Write(static_cast<u64>(vector.size()));
  if (!success) {
    return false;
  }
  for (const T& object : vector) {
    success = Write(object);
    if (!success) {
      return false;
    }
  }
  return true;
}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
bool
RawMemoryWriter::Write(const std::unordered_map<K, V>& map)
{
  bool success = Write(static_cast<u64>(map.size()));
  if (!success) {
    return false;
  }
  for (const auto& entry : map) {
    success = Write<K>(entry.first);
    if (!success) {
      return false;
    }
    success = Write<V>(entry.second);
    if (!success) {
      return false;
    }
  }
  return true;
}

}