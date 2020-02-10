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
// Entry Declaration
// ========================================================================== //

namespace alflib {

/** \class MapEntry
 * \author Filip Björklund
 * \date 23 june 2019 - 15:30
 * \brief Map entry.
 * \details
 * Class that represents an entry in a map.
 */
template<typename K, typename V>
class MapEntry
{
private:
  /** Key **/
  K mKey;
  /** Value**/
  V mValue;

public:
  /** Construct a map entry from a key and a value.
   * \brief Construct map entry from key-value pair.
   * \param key Key to construct entry with.
   * \param value Value to construct entry with.
   */
  MapEntry(K&& key, V&& value);

  /** Construct a map entry with a key and a optional value that may be default
   * constructed.
   * \brief Construct map entry from key and optional value.
   * \param key Key to construct entry with.
   */
  explicit MapEntry(K&& key, const V& value = V());

  /** Construct a map entry with a key and a optional value that may be default
   * constructed.
   * \brief Construct map entry from key and optional value.
   * \param key Key to construct entry with.
   */
  explicit MapEntry(const K& key, const V& value = V());

  /** Returns the key of the map entry.
   * \brief Returns key.
   * \return Key.
   */
  K& GetKey() { return mKey; }

  /** Returns the key of the map entry.
   * \brief Returns key.
   * \return Key.
   */
  const K& GetKey() const { return mKey; }

  /** Returns the value of the map entry.
   * \brief Returns value.
   * \return Value.
   */
  V& GetValue() { return mValue; }

  /** Returns the value of the map entry.
   * \brief Returns value.
   * \return Value.
   */
  const V& GetValue() const { return mValue; }
};

// -------------------------------------------------------------------------- //

template<typename K, typename V>
MapEntry<K, V>::MapEntry(K&& key, V&& value)
  : mKey({ std::forward<K>(key) })
  , mValue({ std::forward<V>(value) })
{}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
MapEntry<K, V>::MapEntry(K&& key, const V& value)
  : mKey({ std::forward<K>(key) })
  , mValue(value)
{}

// -------------------------------------------------------------------------- //

template<typename K, typename V>
MapEntry<K, V>::MapEntry(const K& key, const V& value)
  : mKey({ key })
  , mValue(value)
{}

}