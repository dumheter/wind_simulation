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
#include "alflib/core/assert.hpp"
#include "alflib/collection/array_list.hpp"
#include "alflib/collection/map.hpp"
#include "alflib/core/traits.hpp"

// ========================================================================== //
// ArrayMap Declaration
// ========================================================================== //

namespace alflib {

/** \class ArrayMap
 * \author Filip Björklund
 * \date 23 june 2019 - 15:38
 * \brief Array map.
 * \details
 *
 * Class that represents a simple map implemented as a list in contiguous
 * memory. This list is good for storing small amount of entries where the
 * overhead of complex lookup algorithms would slow it down. However it's not at
 * all feasible for maps of bigger sizes.
 *
 * This map does not have pointer/reference stability due to the nature of using
 * an array-list for internal entry storage.
 */
template<typename K, typename V>
class ArrayMap
{
public:
  /** Entry type **/
  using EntryType = MapEntry<K, V>;
  /** List type **/
  using ListType = ArrayList<EntryType>;
  /** Size type **/
  using SizeType = typename ListType::SizeType;

private:
  /** List of entries **/
  ArrayList<EntryType> mEntries;

public:
  /** Construct an array map from an optional allocator.
   * \brief Construct array map.
   * \param allocator Allocator for allocating entries.
   */
  explicit ArrayMap(Allocator& allocator = DefaultAllocator::Instance());

  /** Returns whether or not the map currently stores an entry with the
   * specified key.
   * \brief Returns whether key is in map.
   * \param key Key to check if present in map.
   * \return True if the key is present in the map otherwise false.
   */
  bool HasKey(const K& key) const;

  /** Returns whether or not the map currently stores an entry with the
   * specified key.
   * \brief Returns whether key is in map.
   * \param key Key to check if present in map.
   * \return True if the key is present in the map otherwise false.
   */
  bool HasKey(K&& key) const;

  /** Remove from the map an entry that has the specified key.
   * \note If no entry exists with the key then nothing is done.
   * \brief Remove entry.
   * \param key Key of the entry to remove.
   */
  void Remove(K&& key);

  /** Returns a reference to the value stored with the specified key. If the key
   * does not yet exist, then it's inserted first.
   * \brief Returns value corresponding to key.
   * \param key Key to lookup value for.
   * \return Value found by looking up key.
   */
  V& At(const K& key);

  /** Returns a reference to the value stored with the specified key. If the key
   * does not yet exist, then it's inserted first.
   * \brief Returns value corresponding to key.
   * \param key Key to lookup value for.
   * \return Value found by looking up key.
   */
  const V& At(const K& key) const;

  /** Returns a reference to the value stored with the specified key. If the key
   * does not yet exist, then it's inserted first.
   * \brief Returns value corresponding to key.
   * \param key Key to lookup value for.
   * \return Value found by looking up key.
   */
  V& At(K&& key);

  /** Returns a reference to the value stored with the specified key. If the key
   * does not yet exist, then it's inserted first.
   * \brief Returns value corresponding to key.
   * \param key Key to lookup value for.
   * \return Value found by looking up key.
   */
  const V& At(K&& key) const;

  /** Returns a reference to the value stored with the specified key. If the key
   * does not yet exist, then it's inserted first.
   * \brief Returns value corresponding to key.
   * \param key Key to lookup value for.
   * \return Value found by looking up key.
   */
  V& operator[](const K& key);

  /** Returns a reference to the value stored with the specified key. If the key
   * does not yet exist, then it's inserted first.
   * \brief Returns value corresponding to key.
   * \param key Key to lookup value for.
   * \return Value found by looking up key.
   */
  V& operator[](K&& key);

  /** Returns the number of entries that are currently stored in the map.
   * \brief Returns size.
   * \return Size in number of entries stored.
   */
  SizeType GetSize() const { return mEntries.GetSize(); }
};

// -------------------------------------------------------------------------- //

template<typename K, typename V>
ArrayMap<K, V>::ArrayMap(Allocator& allocator)
  : mEntries(ArrayList<EntryType>::DEFAULT_CAPACITY, allocator)
{}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
bool
ArrayMap<K, V>::HasKey(const K& key) const
{
  for (EntryType& entry : mEntries) {
    if (entry.GetKey() == key) {
      return true;
    }
  }
  return false;
}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
bool
ArrayMap<K, V>::HasKey(K&& key) const
{
  for (EntryType& entry : mEntries) {
    if (entry.GetKey() == key) {
      return true;
    }
  }
  return false;
}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
void
ArrayMap<K, V>::Remove(K&& key)
{
  for (SizeType i = 0; i < mEntries.GetSize(); i++) {
    if (mEntries[i].GetKey() == key) {
      mEntries.Remove(i);
      return;
    }
  }
}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
V&
ArrayMap<K, V>::At(const K& key)
{
  for (EntryType& entry : mEntries) {
    if (key == entry.GetKey()) {
      return entry.GetValue();
    }
  }
  AlfAssert(false, "Key is not present in map");
  return mEntries[0].GetValue();
}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
const V&
ArrayMap<K, V>::At(const K& key) const
{
  for (EntryType& entry : mEntries) {
    if (key == entry.GetKey()) {
      return entry.GetValue();
    }
  }
  AlfAssert(false, "Key is not present in map");
  return mEntries[0].GetValue();
}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
V&
ArrayMap<K, V>::At(K&& key)
{
  for (EntryType& entry : mEntries) {
    if (key == entry.GetKey()) {
      return entry.GetValue();
    }
  }
  AlfAssert(false, "Key is not present in map");
  return mEntries[0].GetValue();
}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
const V&
ArrayMap<K, V>::At(K&& key) const
{
  for (EntryType& entry : mEntries) {
    if (key == entry.GetKey()) {
      return entry.GetValue();
    }
  }
  AlfAssert(false, "Key is not present in map");
  return mEntries[0].GetValue();
}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
V& ArrayMap<K, V>::operator[](const K& key)
{
  for (EntryType& entry : mEntries) {
    if (key == entry.GetKey()) {
      return entry.GetValue();
    }
  }
  return mEntries.AppendEmplace(key).GetValue();
}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
V& ArrayMap<K, V>::operator[](K&& key)
{
  for (EntryType& entry : mEntries) {
    if (key == entry.GetKey()) {
      return entry.GetValue();
    }
  }
  return mEntries.AppendEmplace(std::forward<K>(key)).GetValue();
}

}