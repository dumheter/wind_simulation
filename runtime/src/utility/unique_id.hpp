#ifndef UNIQUE_ID_HPP_
#define UNIQUE_ID_HPP_

#include "types.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

namespace wind {

using UniqueIdType = u64;

/**
 * Identify something by a value. Each construction of a UniqueID
 * will be unique. Also serializeable.
 *
 * Construct via the generator.
 */
class UniqueId {
public:
  UniqueId() = default;
  // UniqueId() : m_uniqueId(kInvalid) {}

  explicit constexpr UniqueId(UniqueIdType uniqueId) : m_uniqueId(uniqueId) {}

  operator bool() const { return m_uniqueId != kInvalid; }

  bool operator==(const UniqueId &other) const {
    return m_uniqueId == other.m_uniqueId;
  }

  bool operator!=(const UniqueId &other) const { return !(*this == other); }

  bool operator==(UniqueIdType other) const { return m_uniqueId == other; }

  bool operator!=(UniqueIdType other) const { return m_uniqueId != other; }

  UniqueId copy() const { return UniqueId{m_uniqueId}; }

  UniqueIdType raw() const { return m_uniqueId; }

  bool ToBytes(alflib::RawMemoryWriter &mr) const {
    return mr.Write(m_uniqueId);
  }

  static UniqueId FromBytes(alflib::RawMemoryReader &mr) {
    return UniqueId{mr.Read<decltype(m_uniqueId)>()};
  }

public:
  static constexpr UniqueId invalid() { return UniqueId{kInvalid}; }

private:
  static constexpr UniqueIdType kInvalid = 0;

private:
  UniqueIdType m_uniqueId;
};

class UniqueIdGenerator {

public:
  static UniqueId next() {
    auto &gen = instance();
    const UniqueId uid{gen.m_val};
    ++gen.m_val;
    return uid;
  }

private:
  UniqueIdGenerator() : m_val(0) {}

  static UniqueIdGenerator &instance() {
    static UniqueIdGenerator gen{};
    return gen;
  }

  UniqueIdType m_val;
};

} // namespace wind

namespace std {
template <> struct hash<wind::UniqueId> {
  std::size_t operator()(const wind::UniqueId &uid) const noexcept {
    return std::hash<wind::UniqueIdType>{}(uid.raw());
  }
};
} // namespace std

#endif // UNIQUE_ID_HPP_
