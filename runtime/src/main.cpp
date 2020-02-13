#include "app.hpp"
#include "log.hpp"
#include "network/net_common.hpp"
#include "world.hpp"
#include <microprofile/microprofile.h>

int main() {
  MicroProfileInit();
  wind::fixConsole();
  wind::logInfo("¸,ø¤º°`°º¤ø,¸  {}  ¸,ø¤º°`°º¤ø,¸", "W I N D");
  wind::Common::InitNetwork();
  {
    wind::World world{wind::App::Info{"Demo", 1280, 720}};
    world.run();
  }
  wind::Common::KillNetwork();
  return 0;
}
