#include "app.hpp"
#include "log.hpp"
#include "world.hpp"
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingsockets.h>

int main() {
  wind::fixConsole();
  wind::logInfo("¸,ø¤º°`°º¤ø,¸  {}  ¸,ø¤º°`°º¤ø,¸", "W I N D");
  SteamDatagramErrMsg errMsg;
  GameNetworkingSockets_Init(nullptr, errMsg);
  wind::World world{wind::App::Info{"Demo", 1280, 720}};
  world.run();
  return 0;
}
