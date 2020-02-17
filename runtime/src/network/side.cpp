#include "side.hpp"

namespace wind {
std::string SideToString(const Side side) {
  switch (side) {
  case Side::kServer:
    return "server";
  case Side::kClient:
  default:
    return "client";
  }
}
} // namespace wind
