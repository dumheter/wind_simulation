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
    const FieldBase::Dim dim = wind->getDim();
    const f32 cellSize = wind->getCellSize();

    auto parent = csims[i]->SO()->getParent();
    const Vec3F scale{(dim.width - 2) * cellSize, (dim.height - 2) * cellSize,
                      (dim.depth - 2) * cellSize};
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

    // TODO use "blue noise" to chose points to sample
    std::vector<BaseFn::Spline> splines{};
    for (u32 x = 2; x <= dim.width - 1 - 2; x += 2) {
      for (u32 y = 1; y <= dim.height - 1 - 1; y += 2) {
        for (u32 z = 2; z <= dim.depth - 1 - 2; z += 2) {
          auto points =
              bakeAux(wind, Vec3F{static_cast<f32>(x), static_cast<f32>(y),
                                  static_cast<f32>(z)});
          if (!points.empty()) {
            splines.push_back(BaseFn::Spline{
                std::move(points), 2, ObjectBuilder::kSplineSamplesAuto});
          }
        }
      }
    }
    u32 count = 0;
    if (!splines.empty()) {
      count = splines.size();
      cwind->addFunction(BaseFn::fnSplineCollection(std::move(splines)));
    }
    DLOG_INFO("[{:<3}/{}] added {} fn's", i + 1, csims.size(), count);
  }
  DLOG_INFO("..done");
}

static std::vector<Vec3F> bakeAux(VectorField *wind, Vec3F startPos) {
  const FieldBase::Dim dim = wind->getDim();
  const f32 cellSize = wind->getCellSize();

  std::vector<Vec3F> points{};

  Vec3F point = startPos;
  point.x = dclamp(point.x, 0.0f, (dim.width - 1) * cellSize);
  point.y = dclamp(point.y, 0.0f, (dim.height - 1) * cellSize);
  point.z = dclamp(point.z, 0.0f, (dim.depth - 1) * cellSize);
  points.push_back(point);

  constexpr u32 kMaxSteps = 100;
  for (u32 i = 0; i < kMaxSteps; i++) {
    point += wind->sampleNear(point);

    const auto anyAxisOver = [](Vec3F a, Vec3F b, f32 threshold) {
      return std::abs(a.x - b.x) > threshold ||
             std::abs(a.y - b.y) > threshold || std::abs(a.z - b.z) > threshold;
    };
    constexpr f32 kThreshold = 0.05f;
    if (!anyAxisOver(points.back(), point, kThreshold)) {
      DLOG_VERBOSE("early exit, too low wind [{} -> {}]", points.back(), point);
      break;
    }

    points.push_back(point);
    const auto isInside = [](Vec3F point, Vec3F min, Vec3F max) {
      return (point.x >= min.x && point.x <= max.x && point.y >= min.y &&
              point.y <= max.y && point.z >= min.z && point.z <= max.z);
    };
    if (!isInside(point, Vec3F::ZERO,
                  Vec3F{(dim.width - 1) * cellSize, (dim.height - 1) * cellSize,
                        (dim.depth - 1) * cellSize})) {
      if (points.size() < 3) {
        // we interpolate an extra point, from the two existing points
        const Vec3F c = points[1] + (points[1] - points[0]);
        points.push_back(c);
      }
      break;
    }
  }

  if (points.size() > 2) {
    constexpr f32 kPi = 3.141592f;
    const f32 k =
        (kPi * 8.0f) / ((dim.width + dim.height + dim.depth) * cellSize / 3.0f);
    const Vec4F color{std::cos(k * startPos.x) / 2.0f + 0.5f,
                      std::cos(k * startPos.y) / 2.0f + 0.5f,
                      std::cos(k * startPos.z) / 2.0f + 0.5f, 1.0f};
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
