#include "bake.hpp"

#include "shared/math/math.hpp"
#include "shared/scene/component/csim.hpp"
#include "shared/sim/vector_field.hpp"
#include "shared/sim/wind_sim.hpp"
#include "shared/utility/bsprinter.hpp"

#include <Scene/BsSceneManager.h>
#include <alflib/core/assert.hpp>
#include <cmath>
#include <dlog/dlog.hpp>
#include <vector>

#include "shared/scene/builder.hpp"

namespace wind {

static void bakeAux(VectorField *wind, Vec3F startPos);

void bake() {
  DLOG_INFO("Baking");

  auto csims = bs::gSceneManager().findComponents<CSim>(true);
  if (csims.empty()) {
    AlfAssert(false, "failed to find any csim component");
    return;
  }

  WindSimulation *windSim = csims[0]->getSim();
  VectorField *wind = windSim->V();

  const Dim3D dim = wind->getDim();
  const f32 cellSize = wind->getCellSize();
  DLOG_INFO("wind dim ({}, {}, {})", dim.width, dim.height, dim.depth);

  for (u32 x = 4; x <= dim.width - 1 - 4; x += 4) {
    for (u32 y = 1; y <= 2 /*dim.height-1*/; y += 2) {
      for (u32 z = 4; z <= dim.depth - 1 - 4; z += 4) {
        bakeAux(wind, Vec3F{static_cast<f32>(x), static_cast<f32>(y),
                            static_cast<f32>(z)});
      }
    }
  }
  DLOG_INFO("done");
}

static void bakeAux(VectorField *wind, Vec3F startPos) {
  const Dim3D dim = wind->getDim();
  const f32 cellSize = wind->getCellSize();

  std::vector<Vec3F> points{};

  Vec3F point = startPos;
  point.x = dclamp(point.x, 0.0f, (dim.width - 1) * cellSize);
  point.y = dclamp(point.y, 0.0f, (dim.height - 1) * cellSize);
  point.z = dclamp(point.z, 0.0f, (dim.depth - 1) * cellSize);
  points.push_back(point);

  constexpr f32 kSpacing = 0.5f;
  constexpr u32 kMaxSteps = 100;
  for (u32 i = 0; i < kMaxSteps; i++) {
    const auto force = wind->sampleNear(point);
    if (force == Vec3F::ZERO) {
      break;
    }

    point += force;
    point.x = dclamp(point.x, 0.0f, (dim.width - 1) * cellSize);
    point.y = dclamp(point.y, 0.0f, (dim.height - 1) * cellSize);
    point.z = dclamp(point.z, 0.0f, (dim.depth - 1) * cellSize);

    const auto anyAxisOver = [](Vec3F a, Vec3F b, f32 threshold) {
      return std::abs(a.x - b.x) > threshold ||
             std::abs(a.y - b.y) > threshold || std::abs(a.z - b.z) > threshold;
    };

    if (anyAxisOver(points.back(), point, kSpacing) || i + 1 == kMaxSteps) {
      points.push_back(point);
    }
  }

  if (points.size() > 2) {
    const Vec4F color{
        map(startPos.x, 0.0f, dim.width * cellSize, 0.0f, 1.0f),
        map(startPos.y, 0.0f, dim.height * cellSize, 0.0f, 1.0f),
        map(startPos.z, 0.0f, dim.depth * cellSize, 0.0f, 1.0f), 1.0f};
    auto spline =
        ObjectBuilder{ObjectType::kEmpty}
            .withName("bakeSpline")
            .withSpline(points, 2,
                        static_cast<f32>(points.size() * kSpacing * 10.0f),
                        color, Vec3F(0.1f, 0.1f, 0.1f))
            .build();
  }
}

/**
 {
      "name": "box",
      "type": "empty",
      "scale": {"y": 10, "xz": 6},
      "position": { "xz": 12, "y": 5},
      "wind": {
        "occluder": {
          "type": "cube"
        }
      }
    }
 */

} // namespace wind
