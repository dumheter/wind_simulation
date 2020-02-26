#include "base_functions.hpp"

namespace wind {

constexpr BaseFnType deserialize(BaseFnUnderlyingType value) {
  return static_cast<BaseFnType>(value);
}

constexpr BaseFnUnderlyingType serialize(BaseFnType fn) {
  return static_cast<BaseFnUnderlyingType>(fn);
}
} // namespace wind
