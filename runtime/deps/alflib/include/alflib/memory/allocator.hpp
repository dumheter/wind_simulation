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
#include <utility>

// Project headers
#include "alflib/core/common.hpp"

// ========================================================================== //
// Allocator Declaration
// ========================================================================== //

namespace alflib {

/** \class Allocator
 * \author Filip Björklund
 * \date 07 juni 2019 - 20:46
 * \brief Allocator.
 * \details
 * Represents an allocator that can be used to allocate memory. How the memory
 * is allocated depends on the implementation.
 */
class Allocator
{
public:
  /** Default alignment **/
  static constexpr u32 DEFAULT_ALIGNMENT = sizeof(void*);

public:
  /** Default virtual base destructor **/
  virtual ~Allocator() = default;

  /** Allocate memory of the specified size and alignment from the allocator.
   * \note If the size is 0 then null is returned.
   * \pre Alignment must be a power of two.
   * \brief Allocate memory.
   * \param size Size of memory.
   * \param alignment Alignment of memory.
   * \return Allocated memory or null on failure.
   */
  virtual void* Alloc(u64 size, u32 alignment = DEFAULT_ALIGNMENT) = 0;

  /** Free memory that was previously allocated with the allocator.
   * \note The memory pointer can be null.
   * \brief Free memory.
   * \param memory Memory to free.
   */
  virtual void Free(void* memory) = 0;

  /** Safer version of the Allocator::Free function that sets the pointer to
   * null after freeing the data. This lessens the risk of double free or use
   * after free.
   * \brief Safe free.
   * \param memory Memory to free.
   */
  template<typename T>
  void SafeFree(T** memory)
  {
    if (memory) {
      Free(*memory);
      *memory = nullptr;
    }
  }

  /** Allocate and initialize an object of the type T with the arguments
   * forwarded to the constructor.
   * \brief Allocate object.
   * \tparam T Type of object.
   * \tparam ARGS Type of arguments.
   * \param arguments Arguments to constructor.
   * \return Object or null on failure.
   */
  template<typename T, typename... ARGS>
  T* New(ARGS&&... arguments)
  {
    return new (Alloc(sizeof(T), alignof(T)))
      T{ std::forward<ARGS>(arguments)... };
  }

  /** Allocate new array of N number of object of the type T. Each element is
   * default constructed.
   * \brief Allocate new array.
   * \tparam T Type of array elements.
   * \param n Number of object in array.
   * \return Array or null on failure.
   */
  template<typename T>
  T* NewArray(u64 n)
  {
    T* memory = static_cast<T*>(Alloc(sizeof(T) * n, alignof(T)));
    for (u64 i = 0; i < n; i++) {
      new (memory + i) T{};
    }
    return memory;
  }

  /** Delete an object of the type T that was created from this allocator. This
   * calls the destructor and then frees the memory.
   * \brief Delete object.
   * \tparam T Type of object.
   * \param object Object to delete.
   */
  template<typename T>
  void Delete(T* object)
  {
    if (object) {
      object->~T();
      Free(object);
    }
  }
};

}

// ========================================================================== //
// DefaultAllocator Declaration
// ========================================================================== //

namespace alflib {

/** \class DefaultAllocator
 * \author Filip Björklund
 * \date 07 juni 2019 - 21:11
 * \brief Default allocator.
 * \details
 * Represents a default allocator which allocates memory using malloc or similar
 * functions.
 */
class DefaultAllocator : public Allocator
{
public:
  /** \copydoc Allocator::Alloc(u64, u64) **/
  void* Alloc(u64 size, u32 alignment = DEFAULT_ALIGNMENT) override;

  /** \copydoc Allocator::Free(void*) **/
  void Free(void* memory) override;

private:
  /** Default constructor **/
  DefaultAllocator() = default;

  /** Deleted copy-constructor **/
  DefaultAllocator(const DefaultAllocator&) = delete;

  /** Deleted copy-assignment **/
  DefaultAllocator& operator=(const DefaultAllocator&) = delete;

public:
  /** Returns the default allocator singleton.
   * \brief Returns singleton.
   * \return Allocator.
   */
  static DefaultAllocator& Instance();
};

}