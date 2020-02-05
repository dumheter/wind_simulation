#include "app.hpp"
#include "world.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

int main() {
  SteamDatagramErrMsg errMsg;
  GameNetworkingSockets_Init(nullptr, errMsg);
  wind::World world{wind::App::Info{"Demo", 1280, 720}};
  world.run();
  return 0;
}
