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

static std::vector<Vec3F> bakeAux(const VectorField &wind, Vec3F startPos);

void bake() {
  DLOG_INFO("Baking..");

  auto csims = bs::gSceneManager().findComponents<CSim>();
  if (csims.empty()) {
    DLOG_WARNING("There are no wind simulations to bake.");
    return;
  }

  for (u32 i = 0; i < csims.size(); ++i) {
    WindSimulation *sim = csims[i]->getSim();
    const VectorField &vel = sim->V();
    const FieldBase::Dim dim = sim->getDim();
    const Vec3F dimM = sim->getDimM();

    auto parent = csims[i]->SO()->getParent();
    const Vec3F scale{dimM.x - 1, dimM.y - 1, dimM.z - 1};
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
    std::vector<BaseFn::SplineBase> splines{};
    for (u32 x = 2; x <= dim.width - 1 - 2; x += 4) {
      for (u32 y = 1; y <= dim.height - 1 - 1; y += 4) {
        for (u32 z = 2; z <= dim.depth - 1 - 2; z += 4) {
          const Vec3F start = vel.cellToMeter(x, y, z);
          std::vector<Vec3F> points = bakeAux(vel, start);
          if (!points.empty()) {
            splines.push_back(BaseFn::SplineBase{
                std::move(points), 2, ObjectBuilder::kSplineSamplesAuto});
          }
        }
      }
    }
    const u32 count = splines.size();
    if (!splines.empty()) {
      cwind->addFunction(BaseFn::fnSpline(std::move(splines)));
    }
    DLOG_INFO("[{:<3}/{}] added {} fn's", i + 1, csims.size(), count);
  }
  DLOG_INFO("..done");
}

static std::vector<Vec3F> bakeAux(const VectorField &wind, Vec3F startPos) {
  const FieldBase::Dim dim = wind.getDim();
  const Vec3F dimM = wind.getDimM();
  std::vector<Vec3F> points{};

  Vec3F point = startPos;
  point.x = dclamp(point.x, 0.0f, dimM.x - 1);
  point.y = dclamp(point.y, 0.0f, dimM.y - 1);
  point.z = dclamp(point.z, 0.0f, dimM.z - 1);
  points.push_back(point);

  constexpr u32 kMaxSteps = 100;
  for (u32 i = 0; i < kMaxSteps; i++) {
    point += wind.sampleNear(point);

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
    const auto isInside = [](Vec3F p, Vec3F min, Vec3F max) {
      return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y &&
             p.z >= min.z && p.z <= max.z;
    };
    if (!isInside(point, Vec3F::ZERO,
                  Vec3F{dimM.x - 1, dimM.y - 1, dimM.z - 1})) {
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
    const f32 k = (kPi * 8.0f) / ((dimM.x + dimM.y + dimM.z) / 3.0f);
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
