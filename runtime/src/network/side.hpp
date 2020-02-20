#ifndef SIDE_HPP_
#define SIDE_HPP_

#include <string>

namespace wind {

enum class Side : bool { kServer = false, kClient = true };

std::string SideToString(const Side side);
} // namespace wind

#endif // SIDE_HPP_
