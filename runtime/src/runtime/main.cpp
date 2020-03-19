#include "runtime/network/net_common.hpp"
#include "runtime/world.hpp"
#include "shared/app.hpp"
#include "shared/log.hpp"

int main() {
  wind::fixConsole();
  wind::logInfo("¸,ø¤º°`°º¤ø,¸  {}  ¸,ø¤º°`°º¤ø,¸", "W I N D");
  wind::Common::InitNetwork();
  {
    wind::World world{wind::App::Info{"Demo", 720, 480}};
    world.run();
  }
  wind::Common::KillNetwork();
  return 0;
}
