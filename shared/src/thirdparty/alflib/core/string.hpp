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
#include <ostream>
#include <string>

// Library headers
#include "thirdparty/alf_unicode.h"
#include <fmt/ostream.h>

// Project headers
#include "alflib/core/common.hpp"
#include "alflib/memory/unique_pointer.hpp"

// ========================================================================== //
// String Declaration
// ========================================================================== //

namespace alflib {

/** String class **/
class String
{
public:
  /** String iterator **/
  class Iterator
  {
  private:
    /** Underlying string reference **/
    const String& mString;
    /** Offset in string buffer **/
    u64 mOffset;
    /** Current codepoint **/
    u32 mCodepoint;

  public:
    /** Construct iterator for String **/
    Iterator(const String& string, u64 offset);

    /** Proceed in string **/
    void operator++();

    /** Check inequality **/
    bool operator!=(const Iterator& other);

    /** Retrieve reference **/
    u32 operator*();
  };

private:
  /** Underlying string buffer **/
  std::string mBuffer;
  /** Length of the string in codepoints **/
  u32 mLength = 0;

public:
  /** Construct a string from a nul-terminated UTF-8 string.
   * \brief Construct from UTF-8.
   * \param string String to construct from.
   */
  String(const char8* string);

  /** Construct a string from a UTF-8 string of the specified size.
   * \note Size is specified in bytes.
   * \brief Construct from UTF-8.
   * \param string String to construct from.
   */
  String(const char8* string, u32 size);

  /** Construct a string from a nul-terminated UTF-16 string.
   * \brief Construct from UTF-16.
   * \param string String to construct from.
   */
  String(const char16* string);

  /** Construct a string from a std::string.
   * \brief Construct from std::string.
   * \param string String to create from.
   */
  String(const std::string& string);

  /** Construct a string from a single unicode codepoint.
   * \brief Construct from codepoint.
   * \param codepoint Codepoint to construct from.
   */
  String(u32 codepoint);

  /** Copy-constructor **/
  String(const String& string);

  /** Move-constructor **/
  String(String&& string) noexcept;

  /** Default constructor **/
  String() = default;

  /** Destructor **/
  ~String();

  /** Copy-assignment **/
  String& operator=(const String& other);

  /** Move-assignment **/
  String& operator=(String&& other) noexcept;

  /** Returns the codepoint at the specified byte-offset in the string. If the
   * offset is not a start of a valid codepoint then -1 is returned.
   * \brief Returns codepoint at offset.
   * \param offset Offset to return codepoint at.
   * \param width Width of the codepoint.
   * \return Codepoint at offset or -1 if it's not a valid codepoint.
   */
  s64 AtByteOffset(u32 offset, u32& width) const;

  /** Find the index of the first occurance of a specified substring in the
   * string.
   * \brief Find occurance of substring.
   * \param substring Substring to find first occurance of.
   * \return Index of first occurance or -1 if the substring never occurs in the
   * string.
   */
  s64 Find(const String& substring) const;

  /** Returns the index of the first occurance of the specified codepoint.
   * \brief Returns index of first occurance.
   * \param codepoint Codepoint to return first occurance of.
   * \return Index of the first occurance of the codepoint or -1 if the
   * codepoint never occurs in the string.
   */
  s64 IndexOf(u32 codepoint) const;

  /** Returns the index of the last occurance of the specified codepoint.
   * \brief Returns index of last occurance.
   * \param codepoint Codepoint to return last occurance of.
   * \return Index of the last occurance of the codepoint or -1 if the
   * codepoint never occurs in the string.
   */
  s64 LastIndexOf(u32 codepoint) const;

  /** Returns whether or not the string begins with the specified other string.
   * \brief Returns whether string begins with other string.
   * \param string String to check if this string begins with.
   * \return True if this string begins with the other string.
   */
  bool StartsWith(const String& string) const;

  /** Returns whether or not the string begins with the specified codepoint.
   * \brief Returns whether string begins with codepoint.
   * \param codepoint Codepoint to check if string begins with.
   * \return True if the string begins with the specified codepoint otherwise
   * false.
   */
  bool StartsWith(u32 codepoint) const;

  /** Returns whether or not the string ends with the specified other string.
   * \brief Returns whether string ends with other string.
   * \param string String to check if this string ends with.
   * \return True if this string ends with the other string.
   */
  bool EndsWith(const String& string) const;

  /** Returns whether or not the string ends with the specified codepoint.
   * \brief Returns whether string ends with codepoint.
   * \param codepoint Codepoint to check if string ends with.
   * \return True if the string ends with the specified codepoint otherwise
   * false.
   */
  bool EndsWith(u32 codepoint) const;

  /** Replace all occurrences of the
   * \todo Currently the implementation is naïve and not very efficient. A
   * better substring searching algorithm should be used.
   * \brief Replace all occurrences of the string 'from' with the string 'to'.
   * \param from String to replace all occurrences of.
   * \param to String to replace the occurrences with.
   * \return Number of replaced occurrences.
   */
  u32 Replace(const String& from, const String& to);

  /** Remove all occurrences of the specified codepoint from the string.
   * \brief Remove all occurrences of codepoint.
   * \param codepoint Codepoint to remove occurrences of.
   * \return Number of removed codepoints.
   */
  u32 Remove(u32 codepoint);

  /** Returns a substring of the string from the 'from' index and 'count'
   * codepoints ahead.
   * \brief Returns substring.
   * \param from Index to get substring from.
   * \param count Number of codepoint in substring. -1 Means that the entire
   * string from the starting index should be returned.
   * \return Substring.
   */
  String Substring(u64 from, s64 count = -1) const;

  /** Traverse the string and call the specified function with the codepoint
   * and index at each location. The specified arguments are also forwarded to
   * the function.
   * \note The callback arguments are passed after the codepoint (u32) and
   * index (u32).
   * \brief Traverse string.
   * \tparam F Type of callback function.
   * \tparam ARGS Type of arguments to callback function.
   * \param function Callback function.
   * \param arguments Arguments forwarded to the callback function.
   */
  template<typename F, typename... ARGS>
  void ForEach(F&& function, ARGS&&... arguments) const;

  /** Format the string according to the rules of the fmt library. The format is
   * the current string and the arguments are used to format that.
   * \brief
   * \tparam ARGS
   * \param arguments
   * \return
   *
   * \example
   * // "Sum: 2 + 2 = 4"
   * String str = String("Sum: {} + {} = {}").Format(2, "2", 4);
   */
  template<typename... ARGS>
  String Format(ARGS&&... arguments) const;

  /** Concatenate another string at the end of this string.
   * \brief Concatenate string.
   * \param string String to concatenate with.
   */
  void operator+=(const String& string);

  /** Concatenate another string at the end of this string.
   * \brief Concatenate string.
   * \param string String to concatenate with.
   */
  void operator+=(const char8* string);

  /** Returns the codepoint at the specified index in the string.
   * \note This function will traverse the entire string due to the
   * variable-length encoding nature of UTF-8.
   * \brief Returns codepoint at index.
   * \param index Index to get codepoint at.
   * \return Codepoint at index.
   */
  u32 At(u32 index) const;

  /** Returns the codepoint at the specified index in the string.
   * \note This function will traverse the entire string due to the
   * variable-length encoding nature of UTF-8.
   * \brief Returns codepoint at index.
   * \param index Index to get codepoint at.
   * \return Codepoint at index.
   */
  u32 operator[](u32 index) const;

  /** Returns the iterator to the beginning of the string.
   * \brief Returns beginning iterator.
   * \return Begin iterator.
   */
  Iterator Begin() const { return Iterator(*this, 0); }

  /** \copydoc ArrayList::Begin **/
  Iterator begin() const { return Begin(); }

  /** Returns the iterator to the end of the string.
   * \brief Returns ending iterator.
   * \return End iterator.
   */
  Iterator End() const { return Iterator(*this, GetSize()); }

  /** \copydoc ArrayList::End **/
  Iterator end() const { return End(); }

  /** Returns a std::string that represents the same underlying data as this
   * string.
   * \brief Returns std::string.
   * \return String as std::string.
   */
  std::string GetStdString() const { return mBuffer; };

  /** Returns the raw underlying UTF-8 data.
   * \brief Returns UTF-8 data.
   * \return UTF-8 string.
   */
  const char8* GetUTF8() const { return mBuffer.c_str(); }

  /** Convert the underlying string to UTF-16 and returns it.
   * \brief Convert and return UTF-16.
   * \return UTF-16 string.
   */
  UniquePointer<char16[]> GetUTF16() const;

  /** Returns the length of the string in number of codepoints.
   * \note The value returned from this function may not be the same as from the
   * correspoind std::string. For the reason that this string knows about UTF-8.
   * \brief Returns length.
   * \return Length of the string in codepoints.
   */
  u32 GetLength() const { return mLength; }

  /** Returns the size of the string in number of bytes.
   * \note Unlike String::GetLength() this function returns the same value as
   * the corresponding std::string.
   * \note Value returned are always equal to, or greater than, the values
   * returned from the String::GetLength() function.
   * \brief Returns size.
   * \return Size of the string in bytes.
   */
  u32 GetSize() const { return static_cast<u32>(mBuffer.size()); }

public:
  friend std::ostream& operator<<(std::ostream& stream, const String& string);

  friend String operator+(const String& str0, const String& str1);

  friend String operator+(const String& str0, const char8* str1);

  friend String operator+(const char8* str0, const String& str1);

  friend bool operator==(const String& str0, const String& str1);

  friend bool operator==(const String& str0, const char8* str1);

  friend bool operator==(const char8* str0, const String& str1);

  friend bool operator!=(const String& str0, const String& str1);

  friend bool operator!=(const String& str0, const char8* str1);

  friend bool operator!=(const char8* str0, const String& str1);

  template<typename T>
  static String ToString(T value);

  static u32 CodepointWidth(u32 codepoint);
};

// -------------------------------------------------------------------------- //

template<typename F, typename... ARGS>
void
String::ForEach(F&& function, ARGS&&... arguments) const
{
  // Iterate through all codepoints in the source
  u32 codepoint, numBytes;
  u32 index = 0;
  u64 offset = 0;
  while (alfUTF8Decode(GetUTF8(), offset, &codepoint, &numBytes)) {
    // Break on nul-terminator
    if (codepoint == 0) {
      break;
    }

    // Call the callback
    function(codepoint, index, std::forward<ARGS>(arguments)...);

    // Add to offset and index
    offset += numBytes;
    index++;
  }
}

// -------------------------------------------------------------------------- //

template<typename... ARGS>
String
String::Format(ARGS&&... arguments) const
{
  return String(fmt::format(mBuffer, std::forward<ARGS>(arguments)...));
}

// -------------------------------------------------------------------------- //

template<typename T>
String
String::ToString(T value)
{
  return String(std::to_string(value));
}

}

// ========================================================================== //
// String Hash Function
// ========================================================================== //

namespace std {

template<>
struct hash<alflib::String>
{
  std::size_t operator()(const alflib::String& string) const noexcept
  {
    return std::hash<std::string>()(string.GetUTF8());
  }
};

}