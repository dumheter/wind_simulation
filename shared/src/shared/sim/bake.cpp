// MIT License
//
// Copyright (c) 2020 Filip Björklund, Christoffer Gustafsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "bake.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/math/math.hpp"
#include "shared/scene/builder.hpp"
#include "shared/scene/component/csim.hpp"
#include "shared/scene/component/cwind.hpp"
#include "shared/sim/vector_field.hpp"
#include "shared/sim/wind_sim.hpp"
#include "shared/utility/bsprinter.hpp"

#include <cmath>
#include <vector>

#include <dlog/dlog.hpp>

#include <alflib/core/assert.hpp>

#include <Physics/BsPhysics.h>
#include <Scene/BsSceneManager.h>

// ========================================================================== //
// Functions
// ========================================================================== //

namespace wind {

static bool bakerAnyAxisOver(Vec3F a, Vec3F b, f32 threshold) {
  return std::abs(a.x - b.x) > threshold || std::abs(a.y - b.y) > threshold ||
         std::abs(a.z - b.z) > threshold;
}

// -------------------------------------------------------------------------- //

static bool bakerIsInside(Vec3F p, Vec3F min, Vec3F max) {
  return p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y &&
         p.z >= min.z && p.z <= max.z;
}

// -------------------------------------------------------------------------- //

static void bakerBakeAux(std::vector<Vec3F> &points, std::vector<f32> &forces,
                         const VectorField &wind, Vec3F startPos) {
  const Vec3F dimM = wind.getDimM();
  const bs::SPtr<bs::PhysicsScene> &physicsScene =
      bs::gSceneManager().getMainScene()->getPhysicsScene();

  if (!bakerIsInside(startPos, Vec3F::ZERO, dimM)) {
    DLOG_ERROR("cannot bake at point {}, not inside wind simlation {}",
               startPos, dimM);
    return;
  }

  Vec3F point = startPos;
  points.push_back(point);

  constexpr u32 kMaxSteps = 100;
  bool useCollisionSample = false;
  Vec3F collisionSample{};
  for (u32 i = 0; i < kMaxSteps; i++) {
    const auto oldPoint = point;
    const Vec3F force =
        !useCollisionSample ? wind.sampleNear(point) : collisionSample;
    point += force;
    forces.push_back(force.length());

    constexpr f32 kThreshold = 0.05f;
    if (!bakerAnyAxisOver(points.back(), point, kThreshold)) {
      // DLOG_VERBOSE("early exit, too low wind [{} -> {}]", points.back(),
      // point);
      break;
    }

    bs::PhysicsQueryHit hit;
    Vec3F dir = (point - oldPoint);
    dir.normalize();
    const f32 len = distance(point, oldPoint);
    if (physicsScene->rayCast(oldPoint, dir, hit,
                              WindSystem::kWindOccluderLayer, len)) {
      useCollisionSample = true;
      collisionSample = wind.sampleNear(hit.point);
      // const auto samp = wind.sampleNear(hit.point);
      // const auto sampnext = wind.sampleNear(hit.point - (dir * 0.01f));
      // DLOG_INFO("## {:<19} -> {:<19} @ {:<19} with dist {:.1f} | samp {:<19},
      // "
      //           "sampNext {:<19}",
      //           dlog::Format("{}", oldPoint), dlog::Format("{}", point),
      //           dlog::Format("{}", hit.point), hit.distance,
      //           dlog::Format("{}", samp), dlog::Format("{}", sampnext));
      point = hit.point - (dir * 0.01f);
    }

    points.push_back(point);
    if (!bakerIsInside(point, Vec3F::ZERO,
                       Vec3F{dimM.x - 1, dimM.y - 1, dimM.z - 1})) {
      // DLOG_VERBOSE("point left wind simulation volume {} at {}, stopping.",
      //             dimM, point);
      break;
    }
  }

  if (points.size() > 2) {

    // unless early exit, points will have one more item than forces
    if (points.size() == forces.size() + 1) {
      if (bakerIsInside(point, Vec3F::ZERO,
                        Vec3F{dimM.x - 1, dimM.y - 1, dimM.z - 1})) {
        forces.push_back(wind.sampleNear(points.back()).length());
      } else {
        forces.push_back(forces.back());
      }
    }

    // DLOG_INFO("points {}, forces {}", points.size(), forces.size());
    AlfAssert(points.size() == forces.size(),
              "points and forces are not same size");

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
  } else {
    points.clear();
    forces.clear();
  }
}

} // namespace wind

// ========================================================================== //
// Baker Implementation
// ========================================================================== //

namespace wind {

// -------------------------------------------------------------------------- //

void Baker::bakeAll() {
  DLOG_VERBOSE("Starting to bake wind simulations");

  // Find simulations
  const bs::Vector<HCSim> csims = bs::gSceneManager().findComponents<CSim>();
  if (csims.empty()) {
    DLOG_WARNING("There are no wind simulations to bake.");
    return;
  }

  // Bake each simulation object
  for (u32 i = 0; i < csims.size(); ++i) {
    bake(csims[i]->SO(), dlog::Format("baked_wind_{}", i).c_str());
  }

  DLOG_VERBOSE("Done baking");
}

// -------------------------------------------------------------------------- //

bs::HSceneObject Baker::bake(const bs::HSceneObject &obj, const String &name) {
  const HCSim csim = obj->getComponent<CSim>();
  AlfAssert(
      csim != nullptr,
      "Object being baked is required to have a wind simulation component");

  WindSimulation *sim = csim->getSim();
  const VectorField &vel = sim->V();
  const FieldBase::Dim dim = sim->getDim();
  const Vec3F dimM = sim->getDimM();

  const auto parent = csim->SO()->getParent();
  const Vec3F scale{dimM.x - 1, dimM.y - 1, dimM.z - 1};
  const Vec3F pos = parent->getTransform().getPosition() + scale / 2.0f;
  const auto volumeType = WindSystem::VolumeType::kCube;

  const String name_ = name.empty() ? "baked wind" : name;
  auto windSO =
      ObjectBuilder{ObjectType::kEmpty}
          .withName(name_.c_str())
          .withWindVolume(volumeType, pos, scale)
          .withWindSource(std::vector<BaseFn>{}, volumeType, pos, scale)
          .build();
  windSO->setParent(parent);
  auto cwind = windSO->getComponent<CWind>();

  const u32 stepX = 4;
  const u32 stepY = 4;
  const u32 stepZ = 4;
  DLOG_INFO("[BAKE] step size [{}, {}, {}]", stepX, stepY, stepZ);

  // TODO(Filip) use "blue noise" to chose points to sample
  std::vector<BaseFn::SplineBase> splines{};
  u32 particles = 0;
  for (u32 x = 1; x < dim.width - 1; x += stepX) {
    for (u32 y = 1; y < dim.height - 1; y += stepY) {
      for (u32 z = 1; z < dim.depth - 1; z += stepZ) {
        const Vec3F start = vel.cellToMeter(x, y, z);
        std::vector<Vec3F> points;
        std::vector<f32> forces;
        bakerBakeAux(points, forces, vel, start);
        if (!points.empty()) {
          splines.push_back(
              BaseFn::SplineBase{std::move(points), std::move(forces), 2,
                                 ObjectBuilder::kSplineSamplesAuto});
        }
        particles++;
      }
    }
  }
  DLOG_INFO("[BAKE] particles traced: {}", particles);

  u32 pointCount = 0;
  for (const auto &spline : splines) {
    pointCount += spline.points.size();
  }
  if (!splines.empty()) {
    cwind->addFunction(BaseFn::fnSpline(std::move(splines)));
  }

  const auto dimCell = sim->getDim();
  DLOG_INFO("[BAKE] sim size: {}, baked size: {}",
            (dimCell.width + 2) * (dimCell.height + 2) * (dimCell.depth + 2) *
                sizeof(f32) * 3,
            pointCount * sizeof(f32) * 3 + sizeof(f32));

  return windSO;
}

} // namespace wind
