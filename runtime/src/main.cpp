#include "network/net_common.hpp"
#include "shared/app.hpp"
#include "shared/log.hpp"
#include "world.hpp"

int main() {
  wind::fixConsole();
  wind::logInfo("¸,ø¤º°`°º¤ø,¸  {}  ¸,ø¤º°`°º¤ø,¸", "W I N D");
  wind::Common::InitNetwork();
  {
    wind::World world{wind::App::Info{"Demo", 640, 360}};
    world.run();
  }
  wind::Common::KillNetwork();
  return 0;
}
