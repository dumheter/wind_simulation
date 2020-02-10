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
#include "alflib/memory/allocator.hpp"

// ========================================================================== //
// Deleter<T>
// ========================================================================== //

namespace alflib {

/** \class DefaultDeleter
 * \author Filip Björklund
 * \date 10 juni 2019 - 11:17
 * \brief Default deleter.
 * \details
 * Represents a default deleted for UniquePointer.
 */
template<typename T>
class DefaultDeleter
{
public:
  /** Default constructor **/
  DefaultDeleter() = default;

  /** Perform deletion **/
  void operator()(Allocator& allocator, T* pointer) const;
};

// -------------------------------------------------------------------------- //

template<typename T>
void
DefaultDeleter<T>::operator()(Allocator& allocator, T* pointer) const
{
  static_assert(sizeof(T) > 0, "Pointer to incomplete type cannot be deleted");
  allocator.Delete(pointer);
}

}

// ========================================================================== //
// Deleter<T[]>
// ========================================================================== //

namespace alflib {

/** \class DefaultDeleter
 * \author Filip Bj�rklund
 * \date 10 juni 2019 - 11:17
 * \brief Default deleter.
 * \details
 * Represents a default deleted for UniquePointer.
 */
template<typename T>
class DefaultDeleter<T[]>
{
public:
  /** Default constructor **/
  DefaultDeleter() = default;

  /** Perform deletion **/
  void operator()(Allocator& allocator, T* pointer) const;
};

// -------------------------------------------------------------------------- //

template<typename T>
void
DefaultDeleter<T[]>::operator()(Allocator& allocator, T* pointer) const
{
  static_assert(sizeof(T) > 0, "Pointer to incomplete type cannot be deleted");
  allocator.Delete(pointer);
}

}

// ========================================================================== //
// UniquePointer<T>
// ========================================================================== //

namespace alflib {

/** \class UniquePointer
 * \author Filip Bj�rklund
 * \date 10 juni 2019 - 09:41
 * \brief Unique pointer.
 * \details
 * Represents a smart pointer that manages a unique instance of a pointer.
 */
template<typename T, typename D = DefaultDeleter<T>>
class UniquePointer
{
public:
  /** Pointer type **/
  using PointerType = T*;
  /** Reference type **/
  using ReferenceType = T&;

private:
  /** Pointer **/
  T* mPointer = nullptr;
  /** Deleter **/
  D mDeleter;
  /** Allocator **/
  Allocator& mAllocator;

public:
  /** Construct a unique pointer from an existing raw pointer and an allocator.
   * \brief Construct from raw pointer.
   * \param pointer Pointer to construct from.
   * \param allocator Allocator used to delete the object.
   */
  UniquePointer(T* pointer,
                Allocator& allocator = DefaultAllocator::Instance());

  /** Deleted copy constructor **/
  UniquePointer(const UniquePointer&) = delete;

  /** Move constructor **/
  UniquePointer(UniquePointer&& other);

  /** Destructs the unique pointer and deletes the managed pointer aswell.
   * \brief Destruct pointer.
   */
  ~UniquePointer();

  /** Deleted copy assignment **/
  UniquePointer& operator=(const UniquePointer&) = delete;

  /** Move assignment **/
  UniquePointer& operator=(UniquePointer&& other);

  /** Assign a new pointer to be managed. The previous pointer is deleted before
   * the new one is assigned. If the pointer is the one already being managed
   * then this call does nothing.
   * \brief Reassign pointer.
   * \param pointer Pointer to assign.
   * \return Reference to this.
   */
  UniquePointer& operator=(T* pointer);

  /** Returns whether or not the managed object is a valid pointer or nullptr.
   * \brief Returns whether pointer is not null.
   */
  explicit operator bool() const;

  /** Compares the managed objects of two unique pointers for equality.
   * \brief Equality operator.
   * \param other Other pointer to compare with.
   * \return True if the managed objects are equal else false.
   */
  bool operator==(const UniquePointer& other);

  /** Compares the managed objects of two unique pointers for inequality.
   * \brief Inequality operator.
   * \param other Other pointer to compare with.
   * \return True if the managed objects are not equal else false.
   */
  bool operator!=(const UniquePointer& other);

  void Reset(T* pointer = nullptr);

  /** Returns a reference to the managed object.
   * \brief Returns reference to object.
   * \return Object reference.
   */
  T& operator*() { return *mPointer; }

  /** Returns a reference to the managed object.
   * \brief Returns reference to object.
   * \return Object reference.
   */
  const T& operator*() const { return *mPointer; }

  /** Returns a pointer to the managed object.
   * \brief Returns pointer to object.
   * \return Object pointer.
   */
  T* operator->() const { return mPointer; }

  /** Returns a pointer to the managed object.
   * \brief Returns pointer to object.
   * \return Object pointer.
   */
  T* Get() const { return mPointer; }

public:
  /** Make a unique pointer by forwarding the specified arguments to the
   * constructor of the managed object.
   * \brief Make unique pointer.
   * \tparam ARGS Types of arguments to constructor.
   * \param allocator Allocator to allocate the object with.
   * \param arguments Arguments to object constructor.
   * \return Pointer.
   */
  template<typename... ARGS>
  static UniquePointer Make(Allocator& allocator, ARGS&&... arguments);
};

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T, D>::UniquePointer(T* pointer, Allocator& allocator)
  : mPointer(pointer)
  , mDeleter(D{})
  , mAllocator(allocator)
{}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T, D>::UniquePointer(UniquePointer&& other)
  : mAllocator(other.mAllocator)
{
  mPointer = other.mPointer;
  mDeleter = other.mDeleter;
  other.mPointer = nullptr;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T, D>::~UniquePointer()
{
  Reset();
  mDeleter(mAllocator, mPointer);
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T, D>&
UniquePointer<T, D>::operator=(UniquePointer&& other)
{
  if (this != &other) {
    mPointer = other.mPointer;
    mDeleter = other.mDeleter;
    other.mPointer = nullptr;
  }
  return *this;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T, D>&
UniquePointer<T, D>::operator=(T* pointer)
{
  Reset(pointer);
  return *this;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T, D>::operator bool() const
{
  return mPointer != nullptr;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
bool
UniquePointer<T, D>::operator==(const UniquePointer& other)
{
  return mPointer == other.mPointer;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
bool
UniquePointer<T, D>::operator!=(const UniquePointer& other)
{
  return mPointer != other.mPointer;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
void
UniquePointer<T, D>::Reset(T* pointer)
{
  if (mPointer != pointer) {
    mDeleter(mAllocator, mPointer);
    mPointer = pointer;
  }
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
template<typename... ARGS>
UniquePointer<T, D>
UniquePointer<T, D>::Make(Allocator& allocator, ARGS&&... arguments)
{
  T* pointer = allocator.New<T>(std::forward<ARGS>(arguments)...);
  return UniquePointer<T>(pointer);
}

}

// ========================================================================== //
// UniquePointer<T[]>
// ========================================================================== //

namespace alflib {

/** \class UniquePointer
 * \author Filip Bj�rklund
 * \date 10 juni 2019 - 09:41
 * \brief Unique pointer.
 * \details
 * Represents a smart pointer that manages a unique instance of a pointer.
 */
template<typename T, typename D>
class UniquePointer<T[], D>
{
public:
  /** Pointer type **/
  using PointerType = T*;
  /** Reference type **/
  using ReferenceType = T&;

private:
  /** Pointer **/
  T* mPointer = nullptr;
  /** Deleter **/
  D mDeleter;
  /** Allocator **/
  Allocator& mAllocator;

public:
  /** Construct a unique pointer from an existing raw pointer and an allocator.
   * \brief Construct from raw pointer.
   * \param pointer Pointer to construct from.
   * \param allocator Allocator used to delete the object.
   */
  UniquePointer(T* pointer,
                Allocator& allocator = DefaultAllocator::Instance());

  /** Deleted copy constructor **/
  UniquePointer(const UniquePointer&) = delete;

  /** Move constructor **/
  UniquePointer(UniquePointer&& other);

  /** Destructs the unique pointer and deletes the managed pointer aswell.
   * \brief Destruct pointer.
   */
  ~UniquePointer();

  /** Deleted copy assignment **/
  UniquePointer& operator=(const UniquePointer&) = delete;

  /** Move assignment **/
  UniquePointer& operator=(UniquePointer&& other);

  /** Assign a new pointer to be managed. The previous pointer is deleted before
   * the new one is assigned. If the pointer is the one already being managed
   * then this call does nothing.
   * \brief Reassign pointer.
   * \param pointer Pointer to assign.
   * \return Reference to this.
   */
  UniquePointer& operator=(T* pointer);

  /** Returns whether or not the managed object is a valid pointer or nullptr.
   * \brief Returns whether pointer is not null.
   */
  explicit operator bool() const;

  /** Compares the managed objects of two unique pointers for equality.
   * \brief Equality operator.
   * \param other Other pointer to compare with.
   * \return True if the managed objects are equal else false.
   */
  bool operator==(const UniquePointer& other);

  /** Compares the managed objects of two unique pointers for inequality.
   * \brief Inequality operator.
   * \param other Other pointer to compare with.
   * \return True if the managed objects are not equal else false.
   */
  bool operator!=(const UniquePointer& other);

  void Reset(T* pointer = nullptr);

  /** Returns a reference to the managed object.
   * \brief Returns reference to object.
   * \return Object reference.
   */
  T& operator*() { return *mPointer; }

  /** Returns a reference to the managed object.
   * \brief Returns reference to object.
   * \return Object reference.
   */
  const T& operator*() const { return *mPointer; }

  /** Returns a pointer to the managed object.
   * \brief Returns pointer to object.
   * \return Object pointer.
   */
  T* operator->() const { return mPointer; }

  /** Returns a pointer to the managed object.
   * \brief Returns pointer to object.
   * \return Object pointer.
   */
  T* Get() const { return mPointer; }

public:
  static UniquePointer Make(u64 count);
};

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T[], D>::UniquePointer(T* pointer, Allocator& allocator)
  : mPointer(pointer)
  , mDeleter(D{})
  , mAllocator(allocator)
{}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T[], D>::UniquePointer(UniquePointer&& other)
  : mAllocator(other.mAllocator)
{
  mPointer = other.mPointer;
  mDeleter = other.mDeleter;
  other.mPointer = nullptr;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T[], D>::~UniquePointer()
{
  Reset();
  mDeleter(mAllocator, mPointer);
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T[], D>&
UniquePointer<T[], D>::operator=(UniquePointer&& other)
{
  if (this != &other) {
    mPointer = other.mPointer;
    mDeleter = other.mDeleter;
    other.mPointer = nullptr;
  }
  return *this;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T[], D>&
UniquePointer<T[], D>::operator=(T* pointer)
{
  Reset(pointer);
  return *this;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T[], D>::operator bool() const
{
  return mPointer != nullptr;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
bool
UniquePointer<T[], D>::operator==(const UniquePointer& other)
{
  return mPointer == other.mPointer;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
bool
UniquePointer<T[], D>::operator!=(const UniquePointer& other)
{
  return mPointer != other.mPointer;
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
void
UniquePointer<T[], D>::Reset(T* pointer)
{
  if (mPointer != pointer) {
    mDeleter(mAllocator, mPointer);
    mPointer = pointer;
  }
}

// -------------------------------------------------------------------------- //

template<typename T, typename D>
UniquePointer<T[], D>
UniquePointer<T[], D>::Make(u64 count)
{
  return UniquePointer<T, D>(new typename std::remove_extent_t<T>[count]);
}

}
