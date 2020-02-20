#include "packet.hpp"
#include <alflib/core/assert.hpp>
#include <string>

namespace wind {

PayloadIterator::PayloadIterator(ContainerValueType *payload)
    : PayloadIterator(payload, 0) {}

PayloadIterator::PayloadIterator(ContainerValueType *payload,
                                 const std::size_t pos)
    : pos_(pos), payload_(payload) {}

bool PayloadIterator::operator!=(const PayloadIterator &other) const {
  return pos_ != other.pos_;
}

PayloadIterator &PayloadIterator::operator++() {
  ++pos_;
  return *this;
}

PayloadIterator &PayloadIterator::operator--() {
  --pos_;
  return *this;
}

// ============================================================ //

Packet::Packet() : Packet(kDefaultPacketSize) {}

Packet::Packet(const std::size_t size) : size_(kHeaderSize), container_() {
  SetPacketCapacity(kHeaderSize + size);
}

Packet::Packet(const Packet::ValueType *data, const std::size_t data_count)
    : size_(data_count), container_() {
  AlfAssert(data_count >= kHeaderSize,
            "data_count must be larger or equal to header size");
  SetPacketCapacity(data_count);
  SetPacket(data, data_count);
}

Packet::Packet(const char8 *data, const std::size_t data_count)
    : Packet(data_count) {
  ClearHeader();
  SetPayload(data, data_count);
}

Packet::Packet(const alflib::String &string) : Packet(string.GetSize()) {
  ClearHeader();
  SetPayload(string.GetUTF8(), string.GetSize());
}

Packet::Packet(const Packet &other) : Packet(other.GetPacketCapacity()) {
  std::memcpy(GetPacket(), other.GetPacket(), other.GetPacketSize());
  size_ = other.size_;
}

Packet &Packet::operator=(const Packet &other) {
  if (this != &other) {
    std::memcpy(GetPacket(), other.GetPacket(), other.GetPacketSize());
    size_ = other.size_;
  }
  return *this;
}

Packet::Packet(Packet &&other) noexcept
    : size_(other.size_), container_(std::move(other.container_)) {
  other.size_ = 0;
}

Packet &Packet::operator=(Packet &&other) noexcept {
  if (this != &other) {
    size_ = other.size_;
    container_ = std::move(other.container_);
    other.size_ = 0;
  }
  return *this;
}

// ============================================================ //

void Packet::ClearPayload() { size_ = kHeaderSize; }

void Packet::SetPacketCapacity(const std::size_t capacity) {
  container_.resize(capacity);
  if (container_.capacity() > container_.size()) {
    container_.shrink_to_fit();
  }

  // shrink_to_fit may be ignored by some implementations.
  // So make sure we use all bytes allocated.
  if (container_.capacity() > container_.size()) {
    container_.resize(container_.capacity());
  }
}

std::size_t Packet::GetPacketCapacity() const { return container_.capacity(); }

std::size_t Packet::GetPacketSize() const { return size_; }

bool Packet::SetPacket(const Packet::ValueType *data,
                       const std::size_t data_count) {
  AlfAssert(container_.capacity() >= kHeaderSize, "container too small");
  if (container_.capacity() >= data_count) {
    std::memcpy((&container_[0]), data, data_count);
    size_ = data_count;
    return true;
  } else {
    return false;
  }
}

// ============================================================ //

std::size_t Packet::GetHeaderSize() const {
  AlfAssert(container_.capacity() >= kHeaderSize, "container too small");
  return kHeaderSize;
}

void Packet::SetHeader(const PacketHeader header) {
  std::memcpy(&container_[0], &header, kHeaderSize);
}

const PacketHeader *Packet::GetHeader() const {
  return reinterpret_cast<const PacketHeader *>(&container_[0]);
}

PacketHeaderTypes Packet::GetHeaderType() const {
  auto header = GetHeader();
  return header->type;
}

Packet::ValueType *Packet::GetHeaderRaw() { return GetPacket(); }

void Packet::ClearHeader() { std::memset(GetHeaderRaw(), 0, GetHeaderSize()); }

std::size_t Packet::GetPayloadSize() const {
  AlfAssert(size_ >= kHeaderSize, "container too small");
  AlfAssert(container_.capacity() >= kHeaderSize, "container too small");
  return size_ - kHeaderSize;
}

void Packet::SetPayloadSize(const std::size_t size) {
  const std::size_t new_size = GetHeaderSize() + size;
  AlfAssert(new_size <= GetPacketCapacity(),
            "size must be <= PacketCapacity - HeaderSize");
  size_ = new_size;
}

std::size_t Packet::GetPayloadCapacity() const {
  AlfAssert(size_ >= kHeaderSize, "container too small");
  AlfAssert(container_.capacity() >= kHeaderSize, "container too small");
  return GetPacketCapacity() - kHeaderSize;
}

bool Packet::SetPayload(const Packet::ValueType *data,
                        const std::size_t data_count) {
  AlfAssert(container_.capacity() >= kHeaderSize, "container too small");
  if (GetBytesLeft() >= data_count) {
    std::memcpy((&container_[0]) + size_, data, data_count);
    size_ += data_count;
    return true;
  } else {
    return false;
  }
}

bool Packet::SetPayload(const char8 *str, const std::size_t str_len) {
  return SetPayload(reinterpret_cast<const Packet::ValueType *>(str), str_len);
}

const Packet::ValueType *Packet::GetPayload() const {
  AlfAssert(GetPayloadSize() > 0, "container too small");
  return &container_[kHeaderSize];
}

Packet::ValueType *Packet::GetPayload() {
  AlfAssert(GetPayloadSize() > 0, "container too small");
  return &container_[kHeaderSize];
}

Packet::ValueType *Packet::GetRawPayload() {
  AlfAssert(container_.size() > kHeaderSize, "container too small");
  return &container_[kHeaderSize];
}

PayloadIterator Packet::begin() { return PayloadIterator{GetPayload()}; }

PayloadIterator Packet::end() {
  return PayloadIterator{GetPayload(), GetPayloadCapacity()};
}

alflib::String Packet::ToString() const {
  // TODO when alflib is updated remove the std::string
  if (GetPayloadSize() > 0) {
    std::string stdstr{reinterpret_cast<const char8 *>(GetPayload()),
                       GetPayloadSize()};
    alflib::String str{stdstr.c_str()};
    return str;
  }
  return "";
}

MemoryWriter Packet::GetMemoryWriter() {
  MemoryWriter mw{this};
  return mw;
}

alflib::RawMemoryReader Packet::GetMemoryReader() const {
  alflib::RawMemoryReader mr{GetPayload(), GetPayloadSize()};
  return mr;
}

// ============================================================ //

MemoryWriter::MemoryWriter(Packet *packet)
    : mw_(packet->GetRawPayload(), packet->GetPayloadCapacity()),
      packet_(packet), did_finalize(false) {}

MemoryWriter::~MemoryWriter() {
  AlfAssert(did_finalize, "memory writer was destructed without have "
                          "gotten a call to finalize, this is a bug");
}

MemoryWriter::MemoryWriter(MemoryWriter &&other)
    : mw_(other.packet_->GetRawPayload(), other.packet_->GetPayloadCapacity()),
      packet_(other.packet_), did_finalize(false) {
  other.did_finalize = true;
  other.packet_ = nullptr;
}

MemoryWriter &MemoryWriter::operator=(MemoryWriter &&other) noexcept {
  if (this != &other) {
    mw_ = alflib::RawMemoryWriter{other.packet_->GetRawPayload(),
                                  other.packet_->GetPayloadCapacity()};
    packet_ = other.packet_;
    did_finalize = false;
    other.did_finalize = true;
    other.packet_ = nullptr;
  }
  return *this;
}

void MemoryWriter::Finalize() {
  did_finalize = true;
  const std::size_t before = packet_->GetPayloadSize();
  const std::size_t after = before + mw_.GetOffset();
  AlfAssert(after >= before, "offset wrong");
  AlfAssert(after <= packet_->GetPayloadCapacity(), "offset too large");
  packet_->SetPayloadSize(after);
}

} // namespace wind
