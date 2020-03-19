#include "bake.hpp"

#include "shared/math/math.hpp"
#include "shared/scene/component/csim.hpp"
#include "shared/sim/vector_field.hpp"
#include "shared/sim/wind_sim.hpp"
#include "shared/utility/bsprinter.hpp"

#include <Scene/BsSceneManager.h>
#include <cmath>
#include <dlog/dlog.hpp>
#include <vector>

#include "shared/scene/builder.hpp"

namespace wind {


void a(Vec3F startPos);

void bake() {
  for (int x=0; x<10; ++x) {
    for (int y = 0; y < 5; ++y) {
      for (int z = 0; z < 10; ++z) {
        a(Vec3F{1.0f + x, 1.0f + y, 1.0f + z});
      }
    }
  }
}

void a(Vec3F startPos) {

  // -----

  DLOG_INFO("Baking at {}", startPos);

  auto csims = bs::gSceneManager().findComponents<CSim>(true);
  if (csims.empty()) {
    DLOG_ERROR("failed to find any csim components");
    return;
  }

  WindSimulation *windSim = csims[0]->getSim();
  VectorField *wind = windSim->V();

  const Dim3D dim = wind->getDim();
  const f32 cellSize = wind->getCellSize();
  DLOG_INFO("wind dim ({}, {}, {})", dim.width, dim.height, dim.depth);

  std::vector<Vec3F> points{};

  Vec3F point = startPos;
  point.x = dclamp(point.x, 1.0f, dim.width * cellSize);
  point.y = dclamp(point.y, 1.0f, dim.height * cellSize);
  point.z = dclamp(point.z, 1.0f, dim.depth * cellSize);

  points.push_back(point);
  bool filip = false;
  Vec3F filipPoint;
  constexpr u32 kMaxSteps = 30;
  for (u32 i = 0; i < kMaxSteps; i++) {
    // TODO replace with sample all nearby cells

    const s32 x = std::lroundf(point.x * cellSize);
    const s32 y = std::lroundf(point.y * cellSize);
    const s32 z = std::lroundf(point.z * cellSize);
    if (x >= dim.width || y >= dim.height || z >= dim.depth ||
        x < 1 || y < 1 || z < 1) {
      DLOG_INFO("early exit after {} steps, out of bounds: ({}, {}, {})", i, x, y, z);
      break;
    }
    const auto force = wind->get(x, y, z);
    if (!filip) {
      filipPoint = force;
      filip = true;
    }
    point += force;
    points.push_back(point);
  }

  DLOG_INFO("baked {} points", points.size());

  Vec4F color{map(startPos.x, 1.0f, dim.width * cellSize, 0.0f, 1.0f),
        map(startPos.y, 1.0f, dim.height * cellSize, 0.0f, 1.0f),
        map(startPos.z, 1.0f, dim.depth * cellSize, 0.0f, 1.0f),
        1.0f};

  auto spline =
      ObjectBuilder{ObjectType::kEmpty}
          .withName("bakeSpline")
          .withSpline(points, 2, points.size(), color, Vec3F(0.1f, 0.1f, 0.1f))
          .build();
}

} // namespace wind
