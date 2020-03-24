#include "bake.hpp"

#include "shared/math/math.hpp"
#include "shared/scene/builder.hpp"
#include "shared/scene/component/csim.hpp"
#include "shared/scene/component/cwind.hpp"
#include "shared/sim/vector_field.hpp"
#include "shared/sim/wind_sim.hpp"
#include "shared/utility/bsprinter.hpp"

#include <Scene/BsSceneManager.h>
#include <alflib/core/assert.hpp>
#include <cmath>
#include <dlog/dlog.hpp>
#include <vector>

#include "shared/scene/component/cspline.hpp"

namespace wind {

static std::vector<Vec3F> bakeAux(VectorField *wind, Vec3F startPos);

void bake() {
  DLOG_INFO("Baking..");

  auto csims = bs::gSceneManager().findComponents<CSim>();
  if (csims.empty()) {
    DLOG_WARNING("There are no wind simulations to bake.");
    return;
  }

  for (u32 i = 0; i < csims.size(); ++i) {

    WindSimulation *windSim = csims[i]->getSim();
    VectorField *wind = windSim->V();
    const Dim3D dim = wind->getDim();
    const f32 cellSize = wind->getCellSize();

    auto parent = csims[i]->SO()->getParent();
    const Vec3F scale{(dim.width-2) * cellSize, (dim.height-2) * cellSize,
          (dim.depth-2) * cellSize};
    const Vec3F pos = parent->getTransform().getPosition() + scale / 2.0f;
    const auto volumeType = WindSystem::VolumeType::kCube;
    auto windSO =
        ObjectBuilder{ObjectType::kEmpty}
            .withName(dlog::Format("baked wind {}", i).c_str())
            .withWindVolume(volumeType, pos, scale)
            .withWindSource(std::vector<BaseFn>{}, volumeType, pos, scale)
            .build();
    windSO->setParent(parent);
    auto cwind = windSO->getComponent<CWind>();
    // auto cwind =
    //     parent->addComponent<CWind>(WindSystem::VolumeType::kCube, pos,
    //     scale);

    // TODO use "blue noise" to chose points to sample
    for (u32 x = 8; x <= dim.width - 1 - 2; x += 200) {
      for (u32 y = 1; y <= 2 /*dim.height-1*/; y += 400) {
        for (u32 z = 8; z <= dim.depth - 1 - 2; z += 200) {
          auto points =
              bakeAux(wind, Vec3F{static_cast<f32>(x), static_cast<f32>(y),
                                  static_cast<f32>(z)});
          if (!points.empty()) {
            cwind->addFunction(BaseFn::fnSpline(
                std::move(points), 2, ObjectBuilder::kSplineSamplesAuto));
          }
        }
      }
    }
    DLOG_INFO("[{:<3}/{}] added {} fn's", i + 1, csims.size(),
              cwind->getFunctions().size());
  }
  DLOG_INFO("..done");
}

static std::vector<Vec3F> bakeAux(VectorField *wind, Vec3F startPos) {
  const Dim3D dim = wind->getDim();
  const f32 cellSize = wind->getCellSize();

  std::vector<Vec3F> points{};

  Vec3F point = startPos;
  point.x = dclamp(point.x, 0.0f, (dim.width - 1) * cellSize);
  point.y = dclamp(point.y, 0.0f, (dim.height - 1) * cellSize);
  point.z = dclamp(point.z, 0.0f, (dim.depth - 1) * cellSize);
  points.push_back(point);

  constexpr u32 kMaxSteps = 30;
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

    constexpr f32 kSpacing = 0.05f;
    if (!anyAxisOver(points.back(), point, kSpacing)) {
      DLOG_VERBOSE("early exit, too low wind");
      break;
    }
    points.push_back(point);
  }

  if (points.size() > 2) {
    const Vec4F color{map(startPos.x, 0.0f, dim.width * cellSize, 0.0f, 1.0f),
                      map(startPos.y, 0.0f, dim.height * cellSize, 0.0f, 1.0f),
                      map(startPos.z, 0.0f, dim.depth * cellSize, 0.0f, 1.0f),
                      1.0f};
    auto spline =
        ObjectBuilder{ObjectType::kEmpty}
            .withName("bakeSpline")
            .withSpline(points, 2, ObjectBuilder::kSplineSamplesAuto, color)
            .build();
    return points;
  }
  return {};
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
