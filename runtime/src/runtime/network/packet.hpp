#ifndef PACKET_HPP_
#define PACKET_HPP_

#include "runtime/network/connection_id.hpp"
#include "runtime/network/packet_header.hpp"
#include "shared/types.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>
#include <vector>

namespace wind {

using ContainerValueType = u8;

// ============================================================ //
// Memory Writer helpers
// ============================================================ //

class Packet;

struct MemoryWriter {
  explicit MemoryWriter(Packet *packet);
  ~MemoryWriter();

  MemoryWriter(const MemoryWriter &other) = delete;
  MemoryWriter &operator=(const MemoryWriter &other) = delete;
  MemoryWriter(MemoryWriter &&other) noexcept;
  MemoryWriter &operator=(MemoryWriter &&other) noexcept;

  alflib::RawMemoryWriter *operator->() { return &mw_; }
  alflib::RawMemoryWriter *operator*() { return &mw_; }

  /**
   * Will update the packet's payload size, realizing the writes.
   */
  void Finalize();

private:
  alflib::RawMemoryWriter mw_;
  Packet *packet_;
  bool did_finalize;
};

// ============================================================ //
// Payload Iterator
// ============================================================ //

struct PayloadIterator {
  /**
   * Use to construct an iterator pointing to the start of the payload.
   * @param payload Pointer to the payload
   */
  explicit PayloadIterator(ContainerValueType *payload);

  /**
   * To construct an end iterator, set pos to one step beyond the end of the
   * payload.
   * @param payload Pointer to the payload
   * @param pos Where in the payload we are.
   */
  PayloadIterator(ContainerValueType *payload, const std::size_t pos);

  bool operator!=(const PayloadIterator &other) const;

  const ContainerValueType &operator*() const { return payload_[pos_]; }

  ContainerValueType &operator*() { return payload_[pos_]; }

  PayloadIterator &operator++();

  PayloadIterator &operator--();

private:
  std::size_t pos_;
  ContainerValueType *payload_;
};

/**
 * A packet consists of a header and a payload, stored in contiguous memory.
 */
class Packet {
public:
  using ValueType = ContainerValueType;

  // ============================================================ //
  // Lifetime
  // ============================================================ //
public:
  Packet();

  /**
   * Construct a packet.
   * @param size How large payload size to allocate
   */
  explicit Packet(const std::size_t size);

  /**
   * Copy data into a new packet, starting from header
   * @param data_count How many bytes to copy over. Must be more than
   * GetHeaderSize().
   */
  Packet(const ValueType *data, const std::size_t data_count);

  /**
   * Copy data into payload, will write 0's to header.
   */
  Packet(const char8 *data, const std::size_t data_count);

  /**
   * Write string into payload, and write 0's to header
   */
  explicit Packet(const alflib::String &string);

  Packet(const Packet &other);

  Packet &operator=(const Packet &other);

  Packet(Packet &&other) noexcept;

  Packet &operator=(Packet &&other) noexcept;

  // ============================================================ //
  // Packet / General
  // ============================================================ //
public:
  /**
   * Get how many bytes are unused in the packet.
   */
  std::size_t GetBytesLeft() const {
    return GetPacketCapacity() - GetPacketSize();
  }

  /**
   * Set amount of bytes allocated for container. Includes both
   * header and payload.
   */
  void SetPacketCapacity(const std::size_t capacity);

  /**
   * Get total bytes allocated by packet.
   */
  std::size_t GetPacketCapacity() const;

  /**
   * Get total bytes used by packet.
   */
  std::size_t GetPacketSize() const;

  const ValueType *GetPacket() const { return &container_[0]; }

private:
  ValueType *GetPacket() { return &container_[0]; }

public:
  /**
   * Simply take a data source, and write that into our container.
   * @return False if data_count is too large for our packet.
   */
  bool SetPacket(const ValueType *data, const std::size_t data_count);

  /**
   * Get where the packet came from. This is set by the user.
   */
  ConnectionId GetFromConnection() const { return from_; }

  /**
   * Set where the packet came from.
   */
  void SetFromConnection(const ConnectionId from) { from_ = from; }

  // ============================================================ //
  // Header
  // ============================================================ //
public:
  std::size_t GetHeaderSize() const;

  void SetHeader(PacketHeaderTypes header) { SetHeader(PacketHeader{header}); }

private:
  void SetHeader(const PacketHeader header);

public:
  const PacketHeader *GetHeader() const;

  PacketHeaderTypes GetHeaderType() const;

private:
  ValueType *GetHeaderRaw();

public:
  /**
   * Write 0's to header;
   */
  void ClearHeader();

  // ============================================================ //
  // Payload
  // ============================================================ //
public:
  /**
   * Clear the packet, will not write anything to the container.
   */
  void ClearPayload();

  /**
   * Get used bytes by the payload.
   */
  std::size_t GetPayloadSize() const;

  /**
   * Override the payload size and set it yourself.
   *
   * Note: After it is set, the PacketSize will be PacketHeaderSize + size.
   *
   * @pre size must be <= PacketSize.
   */
  void SetPayloadSize(const std::size_t size);

  /**
   * Get the total amount of bytes that can be used by the payload.
   */
  std::size_t GetPayloadCapacity() const;

  /**
   * @return If we could write the entire data into payload.
   */
  bool SetPayload(const ValueType *data, const std::size_t data_count);
  bool SetPayload(const char8 *str, const std::size_t str_len);

  const ValueType *GetPayload() const;

  ValueType *GetPayload();

private:
  friend MemoryWriter;
  ValueType *GetRawPayload();

public:
  PayloadIterator begin();

  PayloadIterator end();

  alflib::String ToString() const;

  /**
   * Use a memory writer to write the packets payload. Pair it with the
   * memory reader to read the memory. It will write to the current
   * payload offset. REMEMBER to call Finialize on the MemoryWriter when done.
   *
   * To get a memory writer pointing to the beginning of the
   * payload, start off with a new packet, or call Clear on the packet.
   *
   * This memory writer will update the packets payload size automatically
   * on destruction
   *
   * Will be invalidated on packet resize.
   */
  MemoryWriter GetMemoryWriter();

  /**
   * Read the payload with memory reader. Pair it with the memory writer
   * to write the memory.
   *
   * Will be invalidated on packet resize.
   */
  alflib::RawMemoryReader GetMemoryReader() const;

  // ============================================================ //
  // Constants
  // ============================================================ //
private:
  static constexpr std::size_t kHeaderSize = sizeof(PacketHeader);
  static constexpr std::size_t kDefaultPacketSize = 1024;

  // ============================================================ //
  // Member variables
  // ============================================================ //
private:
  using PacketContainer = std::vector<ValueType>;

  /**
   * Total used size, header + payload.
   */
  std::size_t size_;

  ConnectionId from_ = kConnectionIdUnknown;

  PacketContainer container_;
};

} // namespace wind

#endif // PACKET_HPP_
