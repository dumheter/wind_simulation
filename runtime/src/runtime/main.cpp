#include "runtime/network/net_common.hpp"
#include "runtime/world.hpp"
#include "shared/app.hpp"
#include <dlog/dlog.hpp>

int main() {
  DLOG_INIT();
  DLOG_SET_LEVEL(dlog::Level::kVerbose);
  DLOG_INFO("¸,ø¤º°`°º¤ø,¸  {}  ¸,ø¤º°`°º¤ø,¸", "W I N D");
  wind::Common::InitNetwork();
  {
    wind::World world{wind::App::Info{"Demo", 720, 480}};
    world.run();
  }
  wind::Common::KillNetwork();
  return 0;
}
