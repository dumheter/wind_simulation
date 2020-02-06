#include "connection_id.hpp"

#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

namespace wind {

static_assert(sizeof(ConnectionId) == sizeof(HSteamNetConnection),
              "steam net connection and ConnectionId type missmatch");

}
