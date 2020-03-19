#include "bake.hpp"

#include "shared/scene/component/csim.hpp"

#include <Scene/BsSceneManager.h>
#include <dlog/dlog.hpp>

namespace wind {

void bake() {
  DLOG_INFO("ayy");
  auto csims = bs::gSceneManager().findComponents<CSim>(true);
  if (csims.empty()) {
    DLOG_ERROR("failed to find any csim components");
    return;
  }

  WindSimulation* windSim = csims[0]->getSim();
}

}
