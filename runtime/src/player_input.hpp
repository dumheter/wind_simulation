#ifndef PLAYER_INPUT_HPP_
#define PLAYER_INPUT_HPP_

#include "common.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

namespace wind {
struct PlayerInput {
  struct Inputs {
    u32 forward : 1;
    u32 back : 1;
    u32 left : 1;
    u32 right : 1;
    u32 fast : 1;
    u32 jump : 1;
    u32 gravity : 1;
  };

  union {
    Inputs inputs;
    u32 value;
  };

  bool operator==(const PlayerInput &other) const {
    return value == other.value;
  }
  bool operator!=(const PlayerInput &other) const { return !(*this == other); }

  bool ToBytes(alflib::RawMemoryWriter &mw) const { return mw.Write(value); }

  static PlayerInput FromBytes(alflib::RawMemoryReader &mr) {
    PlayerInput input{};
    input.value = mr.Read<decltype(value)>();
    return input;
  }
};

struct PlayerCameraInput {
  float radian;

  bool operator==(const PlayerCameraInput &other) const {
    return radian == other.radian;
  }
  bool operator!=(const PlayerCameraInput &other) const {
    return !(*this == other);
  }

  bool ToBytes(alflib::RawMemoryWriter &mw) const { return mw.Write(radian); }

  static PlayerCameraInput FromBytes(alflib::RawMemoryReader &mr) {
    PlayerCameraInput input{};
    input.radian = mr.Read<decltype(radian)>();
    return input;
  }
};

} // namespace wind

#endif // PLAYER_INPUT_HPP_
