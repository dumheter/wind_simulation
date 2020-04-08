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

#include "shared/sim/wind_sim.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/debug/debug_manager.hpp"
#include "shared/math/math.hpp"

#include "microprofile/microprofile.h"

#include <dlog/dlog.hpp>

// ========================================================================== //
// Editor Declaration
// ========================================================================== //

namespace wind {

WindSimulation::WindSimulation(s32 width, s32 height, s32 depth, f32 cellSize)
    : m_width(width * u32(1.0f / cellSize)),
      m_height(height * u32(1.0f / cellSize)),
      m_depth(depth * u32(1.0f / cellSize)), m_cellSize(cellSize),
      m_d(m_width + 2, m_height + 2, m_depth + 2, cellSize),
      m_d0(m_width + 2, m_height + 2, m_depth + 2, cellSize),
      m_v(m_width + 2, m_height + 2, m_depth + 2, cellSize),
      m_v0(m_width + 2, m_height + 2, m_depth + 2, cellSize),
      m_o(m_width + 2, m_height + 2, m_depth + 2, cellSize) {
  // Preconditions
  assert(width != 0 && height != 0 && depth != 0 &&
         "Extent of wind simulation must not be zero in any dimension");

  // Clear
  for (u32 i = 0; i < m_d.getCellCount(); i++) {
    m_d.get(i) = 0.0f;
    m_d0.get(i) = 0.0f;
    m_v.set(i, Vec3F());
    m_v0.set(i, Vec3F());
    m_o.get(i) = false;
  }

  // Initialize
  for (u32 i = 0; i < m_d.getCellCount(); i++) {
    m_d.get(i) = 0.0f;
    m_d0.get(i) = 0.0f;
  }

  DebugManager::setF32(kDebugRunSpeed, 1.0f);

  // setAsTornado();
  setAsVec(Vec3F{0.0f, 0.0f, 1.0f});

  // Post-conditions
  assert(m_d.getDim() == m_v.getDim() &&
         "Density and vector field must have the same dimensions");
  assert(m_d.getDim() == m_o.getDim() &&
         "Density and obstruction field must have the same dimensions");
}

// -------------------------------------------------------------------------- //

void WindSimulation::buildForScene(const bs::SPtr<bs::SceneInstance> &scene,
                                   const bs::Vector3 &position) {
  // Build obstructions for the specified scene. Take the padding into
  // consideration by subtracting it from the position that the collisions are
  // calculated at.
  m_o.buildForScene(scene, position - Vec3F(1, 1, 1) * m_cellSize);

  // Set boundaries to allow seeing blocked vectors in the view before any
  // simulation takes place.
  setBoundary(m_v.getX(), FieldSubKind::kVelX);
  setBoundary(m_v.getY(), FieldSubKind::kVelY);
  setBoundary(m_v.getZ(), FieldSubKind::kVelZ);
  setBoundary(m_v0.getX(), FieldSubKind::kVelX);
  setBoundary(m_v0.getY(), FieldSubKind::kVelY);
  setBoundary(m_v0.getZ(), FieldSubKind::kVelZ);
}

// -------------------------------------------------------------------------- //

void WindSimulation::step(f32 delta) {
  MICROPROFILE_SCOPEI("Sim", "step", MP_ORANGE1);
  if (DebugManager::getBool(kDebugRun)) {
    delta *= DebugManager::getF32(kDebugRunSpeed);
    stepDensity(delta);
    stepVelocity(delta);
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepN(f32 delta, u32 steps) {
  for (u32 i = 0; i < steps; i++) {
    stepDensity(delta);
    stepVelocity(delta);
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepDensity(f32 delta) {
  // Source/Sink
  for (u32 i = 0; i < m_d.getCellCount(); i++) {
    m_d.get(i) += delta * m_d0.get(i);
  }
  if (m_addDensitySource) {
    m_addDensitySource = false;
    m_d.get(1, 1, 1) = 0.5f;
    m_d.get(1, 2, 1) = 0.5f;
    m_d.get(1, 3, 1) = 0.5f;
    m_d.get(1, 4, 1) = 0.5f;
    m_d.get(1, 5, 1) = 0.5f;
  }
  if (m_addDensitySink) {
    m_addDensitySink = false;
    m_d.get(m_width - 3, 1, m_depth - 3) = 0.0f;
    m_d.get(m_width - 3, 2, m_depth - 3) = 0.0f;
    m_d.get(m_width - 3, 3, m_depth - 3) = 0.0f;
    m_d.get(m_width - 3, 4, m_depth - 3) = 0.0f;
    m_d.get(m_width - 3, 5, m_depth - 3) = 0.0f;
  }

  // Diffusion
  if (m_densityDiffusionActive) {
    DensityField::swap(m_d, m_d0);
    diffuse(&m_d, &m_d0, FieldSubKind::kDens, m_diffusion, delta);
  }

  // Advection
  if (m_densityAdvectionActive) {
    DensityField::swap(m_d, m_d0);
    advect(&m_d, &m_d0, &m_v, FieldSubKind::kDens, delta);
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepVelocity(f32 delta) {
  // Source/Sink
  for (u32 i = 0; i < m_v.getCellCount(); i++) {
    Vec3F oldValue = m_v.get(i);
    const Vec3F newValue = oldValue += delta * m_v0.get(i);
    m_v.set(i, newValue);
  }
  if (DebugManager::getBool(kDebugVelocitySource)) {
    DebugManager::setBool(kDebugVelocitySource, false);
    for (s32 x = 11; x < 16; x++) {
      for (s32 y = 3; y < 7; y++) {
        for (s32 z = 4; z < 6; z++) {
          m_v.set(x, y, z, Vec3F(0.0f, 0.0f, 50.0f));
          m_v0.set(x, y, z, Vec3F(0.0f, 0.0f, 50.0f));
        }
        // for (s32 z = 2; z < 4; z++) {
        //  m_v.set(x, y, z, Vec3F(0.0f, 0.0f, 0.0f));
        //  m_v0.set(x, y, z, Vec3F(0.0f, 0.0f, 0.0f));
        //}
      }
    }
  }

  // Diffusion
  if (m_velocityDiffusionActive) {
    VectorField::Comp::swap(m_v0.getX(), m_v.getX());
    diffuse(m_v.getX(), m_v0.getX(), FieldSubKind::kVelX, m_viscosity, delta);
    VectorField::Comp::swap(m_v0.getY(), m_v.getY());
    diffuse(m_v.getY(), m_v0.getY(), FieldSubKind::kVelY, m_viscosity, delta);
    VectorField::Comp::swap(m_v0.getZ(), m_v.getZ());
    diffuse(m_v.getZ(), m_v0.getZ(), FieldSubKind::kVelZ, m_viscosity, delta);
    project(m_v.getX(), m_v.getY(), m_v.getZ(), m_v0.getX(), m_v0.getY());
  }

  // Advection
  if (m_velocityAdvectionActive) {
    VectorField::Comp::swap(m_v0.getX(), m_v.getX());
    VectorField::Comp::swap(m_v0.getY(), m_v.getY());
    VectorField::Comp::swap(m_v0.getZ(), m_v.getZ());
    advect(m_v.getX(), m_v0.getX(), &m_v0, FieldSubKind::kVelX, delta);
    advect(m_v.getY(), m_v0.getY(), &m_v0, FieldSubKind::kVelY, delta);
    advect(m_v.getZ(), m_v0.getZ(), &m_v0, FieldSubKind::kVelZ, delta);
    project(m_v.getX(), m_v.getY(), m_v.getZ(), m_v0.getX(), m_v0.getY());
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::paint(Painter &painter, const bs::Vector3 &offset) const {
  // Do we draw?
  if (!DebugManager::getBool(kDebugPaintKey)) {
    return;
  }

  // Draw correct type
  const FieldKind kind =
      static_cast<FieldKind>(DebugManager::getS32(kDebugFieldTypeKey));
  switch (kind) {
  case FieldKind::kDens: {
    m_d.paint(painter, offset, Vec3F(1, 1, 1));
    break;
  }
  case FieldKind::kVel: {
    m_v.paintWithObstr(painter, m_o, offset, Vec3F(1, 1, 1));
    break;
  }
  case FieldKind::kObstr: {
    m_o.paint(painter, offset, Vec3F(1, 1, 1));
    break;
  }
  default:
    break;
  }

  // Draw frame
  if (DebugManager::getBool(kDebugPaintFrameKey)) {
    m_d.paintFrame(painter, offset);
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::setAsTornado() {
  const Vec3F posCenter(m_width / 2.0f, 0.0f, m_depth / 2.0f);

  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 i = 1; i <= m_width; i++) {
        const Vec3F pos(f32(i), 0.0f, f32(k));
        Vec3F vec = (pos - posCenter);
        const f32 dist = vec.normalize();
        const f32 tmp = vec.x;
        vec.x = vec.z;
        vec.z = -tmp;
        vec.y = 0.1f;

        Vec3F res = vec * f32(j) / clamp(dist, 1.0f, 10.0f);
        res.y = clamp(res.y, -5.0f, 5.0f);
        res.x = clamp(res.x, -5.0f, 5.0f);
        res.z = clamp(res.z, -5.0f, 5.0f);

        m_v.set(i, j, k, res);
        m_v0.set(i, j, k, res);
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::setAsVec(Vec3F v) {
  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 i = 1; i <= m_width; i++) {
        m_v.set(i, j, k, v);
        m_v0.set(i, j, k, v);
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::gaussSeidel(Field<f32> *f, Field<f32> *f0,
                                 FieldSubKind edge, f32 a, f32 c) {
  // Gauss-Seidel relaxation
  for (u32 l = 0; l < GAUSS_SEIDEL_STEPS; l++) {
    // Diffuse with neighbors
    for (s32 k = 1; k <= m_depth; k++) {
      for (s32 j = 1; j <= m_height; j++) {
        for (s32 i = 1; i <= m_width; i++) {
          const f32 comb = f->get(i - 1, j, k) + f->get(i + 1, j, k) +
                           f->get(i, j - 1, k) + f->get(i, j + 1, k) +
                           f->get(i, j, k - 1) + f->get(i, j, k + 1);
          f->get(i, j, k) = (f0->get(i, j, k) + a * comb) / c;
        }
      }
    }
    setBoundary(f, edge);
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::diffuse(Field<f32> *f, Field<f32> *f0, FieldSubKind edge,
                             f32 coeff, f32 delta) {
  const s32 maxDim = wind::maxValue(m_width, m_height, m_depth);
  const s32 cubic = maxDim * maxDim * maxDim;
  const f32 a = delta * coeff * cubic;
  const f32 c = 1.0f + 6.0f * a;
  gaussSeidel(f, f0, edge, a, c);
}

// -------------------------------------------------------------------------- //

void WindSimulation::advect(Field<f32> *f, Field<f32> *f0,
                            VectorField *vecField, FieldSubKind edge,
                            f32 delta) {
  const s32 maxDim = wind::maxValue(m_width, m_height, m_depth);
  const f32 deltaX = delta * maxDim;
  const f32 deltaY = delta * maxDim;
  const f32 deltaZ = delta * maxDim;

  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 i = 1; i <= m_width; i++) {
        const Vec3F vec = vecField->get(i, j, k);

        const f32 x = clamp(i - deltaX * vec.x, 0.5f, m_width + 0.5f);
        const s32 i0 = s32(x);
        const s32 i1 = i0 + 1;
        const f32 s1 = x - i0;
        const f32 s0 = 1 - s1;

        const f32 y = clamp(j - deltaY * vec.y, 0.5f, m_height + 0.5f);
        const s32 j0 = s32(y);
        const s32 j1 = j0 + 1;
        const f32 t1 = y - j0;
        const f32 t0 = 1 - t1;

        const f32 z = clamp(k - deltaZ * vec.z, 0.5f, m_depth + 0.5f);
        const s32 k0 = s32(z);
        const s32 k1 = k0 + 1;
        const f32 u1 = z - k0;
        const f32 u0 = 1 - u1;

        // Interpolate between cells
        const f32 tu0 =
            t0 * u0 * f0->get(i0, j0, k0) + t1 * u0 * f0->get(i0, j1, k0) +
            t0 * u1 * f0->get(i0, j0, k1) + t1 * u1 * f0->get(i0, j1, k1);
        const f32 tu1 =
            t0 * u0 * f0->get(i1, j0, k0) + t1 * u0 * f0->get(i1, j1, k0) +
            t0 * u1 * f0->get(i1, j0, k1) + t1 * u1 * f0->get(i1, j1, k1);
        f->get(i, j, k) = s0 * tu0 + s1 * tu1;
      }
    }
  }

  setBoundary(f, edge);
}

// -------------------------------------------------------------------------- //

void WindSimulation::project(Field<f32> *u, Field<f32> *v, Field<f32> *w,
                             Field<f32> *prj, Field<f32> *div) {

  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 i = 1; i <= m_width; i++) {
        const f32 comb = (u->get(i + 1, j, k) - u->get(i - 1, j, k)) / m_width +
                         (v->get(i, j + 1, k) - v->get(i, j - 1, k)) / m_width +
                         (w->get(i, j, k + 1) - w->get(i, j, k - 1)) / m_width;
        div->get(i, j, k) = -1.0f / 3.0f * comb;
        prj->get(i, j, k) = 0;
      }
    }
  }

  setBoundary(div, FieldSubKind::kDens);
  setBoundary(prj, FieldSubKind::kDens);

  gaussSeidel(prj, div, FieldSubKind::kDens, 1.0f, 6.0f);

  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 i = 1; i <= m_width; i++) {
        u->get(i, j, k) -=
            0.5f * m_width * (prj->get(i + 1, j, k) - prj->get(i - 1, j, k));
        v->get(i, j, k) -=
            0.5f * m_width * (prj->get(i, j + 1, k) - prj->get(i, j - 1, k));
        w->get(i, j, k) -=
            0.5f * m_width * (prj->get(i, j, k + 1) - prj->get(i, j, k - 1));
      }
    }
  }

  setBoundary(u, FieldSubKind::kVelX);
  setBoundary(v, FieldSubKind::kVelY);
  setBoundary(w, FieldSubKind::kVelZ);
}

// -------------------------------------------------------------------------- //

void WindSimulation::setBoundary(Field<f32> *f, FieldSubKind edge) {
  const bool isX = edge == FieldSubKind::kVelX;
  const bool isY = edge == FieldSubKind::kVelY;
  const bool isZ = edge == FieldSubKind::kVelZ;

  for (s32 i = 1; i <= m_width; i++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 k = 1; k <= m_depth; k++) {
        const f32 curr = f->get(i, j, k);

        if (isX) {
          const f32 vMin = m_o.get(i - 1, j, k) ? 0.0f : curr;
          const f32 vMax = m_o.get(i + 1, j, k) ? 0.0f : curr;
          f->get(i, j, k) = wind::clamp(curr, vMin, vMax);
        }
        if (isY) {
          const f32 vMin = m_o.get(i, j - 1, k) ? 0.0f : curr;
          const f32 vMax = m_o.get(i, j + 1, k) ? 0.0f : curr;
          f->get(i, j, k) = wind::clamp(curr, vMin, vMax);
        }
        if (isZ) {
          const f32 vMin = m_o.get(i, j, k - 1) ? 0.0f : curr;
          const f32 vMax = m_o.get(i, j, k + 1) ? 0.0f : curr;
          f->get(i, j, k) = wind::clamp(curr, vMin, vMax);
        }
      }
    }
  }

  // X-Y faces
  for (s32 i = 1; i <= m_width; i++) {
    for (s32 j = 1; j <= m_height; j++) {
      f->get(i, j, 0) = isZ ? -f->get(i, j, 1) : f->get(i, j, 1);
      f->get(i, j, m_depth + 1) =
          isZ ? -f->get(i, j, m_depth) : f->get(i, j, m_depth);
    }
  }

  // Y-Z faces
  for (s32 i = 1; i <= m_height; i++) {
    for (s32 j = 1; j <= m_depth; j++) {
      f->get(0, i, j) = isX ? -f->get(1, i, j) : f->get(1, i, j);
      f->get(m_width + 1, i, j) =
          isX ? -f->get(m_width, i, j) : f->get(m_width, i, j);
    }
  }

  // X-Z faces
  for (s32 i = 1; i <= m_width; i++) {
    for (s32 j = 1; j <= m_depth; j++) {
      f->get(i, 0, j) = isY ? -f->get(i, 1, j) : f->get(i, 1, j);
      f->get(i, m_height + 1, j) =
          isY ? -f->get(i, m_height, j) : f->get(i, m_height, j);
    }
  }

  // X edges
  for (s32 i = 1; i <= m_width; i++) {
    f->get(i, 0, 0) = 1.0f / 2.0f * (f->get(i, 1, 0) + f->get(i, 0, 1));
    f->get(i, m_height + 1, 0) =
        1.0f / 2.0f * (f->get(i, m_height, 0) + f->get(i, m_height + 1, 1));
    f->get(i, 0, m_depth + 1) =
        1.0f / 2.0f * (f->get(i, 0, m_depth) + f->get(i, 1, m_depth + 1));
    f->get(i, m_height + 1, m_depth + 1) =
        1.0f / 2.0f *
        (f->get(i, m_height, m_depth + 1) + f->get(i, m_height + 1, m_depth));
  }

  // Y edges
  for (s32 i = 1; i <= m_height; i++) {
    f->get(0, i, 0) = 1.0f / 2.0f * (f->get(1, i, 0) + f->get(0, i, 1));
    f->get(m_width + 1, i, 0) =
        1.0f / 2.0f * (f->get(m_width, i, 0) + f->get(m_width + 1, i, 1));
    f->get(0, i, m_depth + 1) =
        1.0f / 2.0f * (f->get(0, i, m_depth) + f->get(1, i, m_depth + 1));
    f->get(m_width + 1, i, m_depth + 1) =
        1.0f / 2.0f *
        (f->get(m_width, i, m_depth + 1) + f->get(m_width + 1, i, m_depth));
  }

  // Z edges
  for (s32 i = 1; i <= m_depth; i++) {
    f->get(0, 0, i) = 1.0f / 2.0f * (f->get(0, 1, i) + f->get(1, 0, i));
    f->get(0, m_height + 1, i) =
        1.0f / 2.0f * (f->get(0, m_height, i) + f->get(1, m_height + 1, i));
    f->get(m_width + 1, 0, i) =
        1.0f / 2.0f * (f->get(m_width, 0, i) + f->get(m_width + 1, 1, i));
    f->get(m_width + 1, m_height + 1, i) =
        1.0f / 2.0f *
        (f->get(m_width + 1, m_height, i) + f->get(m_width, m_height + 1, i));
  }

  // Corners
  f->get(0, 0, 0) =
      1.0f / 3.0f * (f->get(1, 0, 0) + f->get(0, 1, 0) + f->get(0, 0, 1));
  f->get(0, m_height + 1, 0) =
      1.0f / 3.0f *
      (f->get(1, m_height + 1, 0) + f->get(0, m_height, 0) +
       f->get(0, m_height + 1, 1));

  f->get(m_width + 1, 0, 0) =
      1.0f / 3.0f *
      (f->get(m_width, 0, 0) + f->get(m_width + 1, 1, 0) +
       f->get(m_width + 1, 0, 1));
  f->get(m_width + 1, m_height + 1, 0) =
      1.0f / 3.0f *
      (f->get(m_width, m_height + 1, 0) + f->get(m_width + 1, m_height, 0) +
       f->get(m_width + 1, m_height + 1, 1));
  f->get(0, 0, m_depth + 1) =
      1.0f / 3.0f *
      (f->get(1, 0, m_depth + 1) + f->get(0, 1, m_depth + 1) +
       f->get(0, 0, m_depth));
  f->get(0, m_height + 1, m_depth + 1) =
      1.0f / 3.0f *
      (f->get(1, m_height + 1, m_depth + 1) + f->get(0, m_height, m_depth + 1) +
       f->get(0, m_height + 1, m_depth));
  f->get(m_width + 1, 0, m_depth + 1) =
      1.0f / 3.0f *
      (f->get(m_width, 0, m_depth + 1) + f->get(m_width + 1, 1, m_depth + 1) +
       f->get(m_width + 1, 0, m_depth));
  f->get(m_width + 1, m_height + 1, m_depth + 1) =
      1.0f / 3.0f *
      (f->get(m_width, m_height + 1, m_depth + 1) +
       f->get(m_width + 1, m_height, m_depth + 1) +
       f->get(m_width + 1, m_height + 1, m_depth));
}

} // namespace wind
