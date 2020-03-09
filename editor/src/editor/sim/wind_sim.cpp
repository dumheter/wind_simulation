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

#include "editor/sim/wind_sim.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "shared/log.hpp"
#include "shared/math/math.hpp"

// ========================================================================== //
// Editor Declaration
// ========================================================================== //

namespace wind {

WindSimulation::WindSimulation(s32 width, s32 height, s32 depth, f32 cellSize)
    : m_width(width * u32(1.0f / cellSize)),
      m_height(height * u32(1.0f / cellSize)),
      m_depth(depth * u32(1.0f / cellSize)), m_cellSize(cellSize) {
  // Preconditions
  assert(width != 0 && height != 0 && depth != 0 &&
         "Extent of wind simulation must not be zero in any dimension");

  // Allocate fields
  m_d = new DensityField(m_width + 2, m_height + 2, m_depth + 2, cellSize);
  m_d0 = new DensityField(m_width + 2, m_height + 2, m_depth + 2, cellSize);
  m_v = new VectorField(m_width + 2, m_height + 2, m_depth + 2, cellSize);
  m_v0 = new VectorField(m_width + 2, m_height + 2, m_depth + 2, cellSize);
  m_o = new ObstructionField(m_width + 2, m_height + 2, m_depth + 2, cellSize);

  // Clear
  for (u32 i = 0; i < m_d->getDataSize(); i++) {
    m_d->get(i) = 0.0f;
    m_d0->get(i) = 0.0f;
    m_v->set(i, Vec3F());
    m_v0->set(i, Vec3F());
    m_o->get(i) = false;
  }

  // Initialize
  for (u32 i = 0; i < m_d->getDataSize(); i++) {
    m_d->get(i) = 0.0f;
    m_d0->get(i) = 0.0f;
  }
  // m_v->set(22, 12, 16, Vec3F(1.0f, 0.0f, 0.1f));
  // m_v0->set(22, 12, 16, Vec3F(1.0f, 0.0f, 0.1f));

  // Post-conditions
  assert(m_d->getDim() == m_v->getDim() &&
         "Density and vector field must have the same dimensions");
  assert(m_d->getDim() == m_o->getDim() &&
         "Density and obstruction field must have the same dimensions");
}

// -------------------------------------------------------------------------- //

WindSimulation::~WindSimulation() {
  delete m_d;
  delete m_d0;
  delete m_v;
  delete m_v0;
  delete m_o;
}

// -------------------------------------------------------------------------- //

void WindSimulation::buildForScene(const bs::SPtr<bs::SceneInstance> &scene,
                                   const bs::Vector3 &position) {
  // Build obstructions for the specified scene. Take the padding into
  // consideration by subtracting it from the position that the collisions are
  // calculated at.
  m_o->buildForScene(scene, position - Vec3F(1, 1, 1) * m_cellSize);

  // Set boundaries to allow seeing blocked vectors in the view before any
  // simulation takes place.
  setBoundary(m_v->getX(), EdgeKind::VELOCITY_X);
  setBoundary(m_v->getY(), EdgeKind::VELOCITY_Y);
  setBoundary(m_v->getZ(), EdgeKind::VELOCITY_Z);
  setBoundary(m_v0->getX(), EdgeKind::VELOCITY_X);
  setBoundary(m_v0->getY(), EdgeKind::VELOCITY_Y);
  setBoundary(m_v0->getZ(), EdgeKind::VELOCITY_Z);
}

// -------------------------------------------------------------------------- //

void WindSimulation::step(f32 delta) {
  stepDensity(delta);
  stepVelocity(delta);
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
  for (u32 i = 0; i < m_d->getDataSize(); i++) {
    m_d->get(i) += delta * m_d0->get(i);
  }
  if (m_addDensitySource) {
    m_addDensitySource = false;
    m_d->get(1, 1, 1) = 0.5f;
    m_d->get(1, 2, 1) = 0.5f;
    m_d->get(1, 3, 1) = 0.5f;
    m_d->get(1, 4, 1) = 0.5f;
    m_d->get(1, 5, 1) = 0.5f;
  }
  if (m_addDensitySink) {
    m_addDensitySink = false;
    m_d->get(m_width - 3, 1, m_depth - 3) = 0.0f;
    m_d->get(m_width - 3, 2, m_depth - 3) = 0.0f;
    m_d->get(m_width - 3, 3, m_depth - 3) = 0.0f;
    m_d->get(m_width - 3, 4, m_depth - 3) = 0.0f;
    m_d->get(m_width - 3, 5, m_depth - 3) = 0.0f;
  }

  // Diffusion
  if (m_densityDiffusionActive) {
    DensityField::swap(m_d, m_d0);
    diffuse(m_d, m_d0, EdgeKind::DENSITY, m_diffusion, delta);
  }

  // Advection
  if (m_densityAdvectionActive) {
    DensityField::swap(m_d, m_d0);
    advect(m_d, m_d0, m_v, EdgeKind::DENSITY, delta);
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepVelocity(f32 delta) {
  // Source/Sink
  for (u32 i = 0; i < m_v->getDataSize(); i++) {
    Vec3F oldValue = m_v->get(i);
    Vec3F newValue = oldValue += delta * m_v0->get(i);
    m_v->set(i, newValue);
  }
  if (m_addVelocitySource) {
    m_addVelocitySource = false;
    if (false) { // Left-side source
      for (s32 z = 2; z < 7; z++) {
        for (s32 y = 2; y < 7; y++) {
          m_v->set(5, y, z, Vec3F(100.0f, 0.0f, 0.0f));
          m_v0->set(5, y, z, Vec3F(100.0f, 0.0f, 0.0f));
        }
      }
    } else { // Center source
      for (s32 z = 14; z < 18; z++) {
        for (s32 y = 10; y < 14; y++) {
          m_v->set(5, y, z, Vec3F(100.0f, 0.2f, 0.0f));
          m_v0->set(5, y, z, Vec3F(100.0f, 0.2f, 0.0f));
        }
      }
    }
  }

  // Diffusion
  if (m_velocityDiffusionActive) {
    VectorField::Comp::swap(m_v0->getX(), m_v->getX());
    diffuse(m_v->getX(), m_v0->getX(), EdgeKind::VELOCITY_X, m_viscosity,
            delta);
    VectorField::Comp::swap(m_v0->getY(), m_v->getY());
    diffuse(m_v->getY(), m_v0->getY(), EdgeKind::VELOCITY_Y, m_viscosity,
            delta);
    VectorField::Comp::swap(m_v0->getZ(), m_v->getZ());
    diffuse(m_v->getZ(), m_v0->getZ(), EdgeKind::VELOCITY_Z, m_viscosity,
            delta);
    project(m_v->getX(), m_v->getY(), m_v->getZ(), m_v0->getX(), m_v0->getY());
  }

  // Advection
  if (m_velocityAdvectionActive) {
    VectorField::Comp::swap(m_v0->getX(), m_v->getX());
    VectorField::Comp::swap(m_v0->getY(), m_v->getY());
    VectorField::Comp::swap(m_v0->getZ(), m_v->getZ());
    advect(m_v->getX(), m_v0->getX(), m_v0, EdgeKind::VELOCITY_X, delta);
    advect(m_v->getY(), m_v0->getY(), m_v0, EdgeKind::VELOCITY_Y, delta);
    advect(m_v->getZ(), m_v0->getZ(), m_v0, EdgeKind::VELOCITY_Z, delta);
    project(m_v->getX(), m_v->getY(), m_v->getZ(), m_v0->getX(), m_v0->getY());
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::debugDraw(FieldKind kind, const bs::Vector3 &offset,
                               bool drawFrame) {
  bs::DebugDraw::instance().clear();

  // Draw correct type
  switch (kind) {
  case FieldKind::DENSITY: {
    m_d->debugDraw(offset, drawFrame, Vec3F(1, 1, 1));
    break;
  }
  case FieldKind::VELOCITY: {
    m_v->debugDraw(offset, m_o, drawFrame, Vec3F(1, 1, 1));
    break;
  }
  case FieldKind::OBSTRUCTION: {
    m_o->debugDraw(offset, drawFrame, Vec3F(1, 1, 1));
    break;
  }
  default:
    break;
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::gaussSeidel(Field<f32> *f, Field<f32> *f0, EdgeKind edge,
                                 f32 a, f32 c) {
  // Gauss-Seidel relaxation
  for (u32 l = 0; l < GAUSS_SEIDEL_STEPS; l++) {
    // Diffuse with neighbors
    for (s32 k = 1; k <= m_depth; k++) {
      for (s32 j = 1; j <= m_height; j++) {
        for (s32 i = 1; i <= m_width; i++) {
          f32 comb = f->get(i - 1, j, k) + f->get(i + 1, j, k) +
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

void WindSimulation::diffuse(Field<f32> *f, Field<f32> *f0, EdgeKind edge,
                             f32 coeff, f32 delta) {
  s32 maxDim = wind::maxValue(m_width, m_height, m_depth);
  s32 cubic = maxDim * maxDim * maxDim;
  f32 a = delta * coeff * cubic;
  f32 c = 1.0f + 6.0f * a;
  gaussSeidel(f, f0, edge, a, c);
}

// -------------------------------------------------------------------------- //

void WindSimulation::advect(Field<f32> *f, Field<f32> *f0,
                            VectorField *vecField, EdgeKind edge, f32 delta) {
  s32 maxDim = wind::maxValue(m_width, m_height, m_depth);
  f32 deltaX = delta * maxDim;
  f32 deltaY = delta * maxDim;
  f32 deltaZ = delta * maxDim;

  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 i = 1; i <= m_width; i++) {
        const Vec3F vec = vecField->get(i, j, k);
        f32 x = clamp(i - deltaX * vec.x, 0.5f, m_width + 0.5f);
        s32 i0 = s32(x);
        s32 i1 = i0 + 1;
        f32 s1 = x - i0;
        f32 s0 = 1 - s1;

        f32 y = clamp(j - deltaY * vec.y, 0.5f, m_height + 0.5f);
        s32 j0 = s32(y);
        s32 j1 = j0 + 1;
        f32 t1 = y - j0;
        f32 t0 = 1 - t1;

        f32 z = clamp(k - deltaZ * vec.z, 0.5f, m_depth + 0.5f);
        s32 k0 = s32(z);
        s32 k1 = k0 + 1;
        f32 u1 = z - k0;
        f32 u0 = 1 - u1;

        // Interpolate between cells
        f32 tu0 = t0 * u0 * f0->get(i0, j0, k0) +
                  t1 * u0 * f0->get(i0, j1, k0) +
                  t0 * u1 * f0->get(i0, j0, k1) + t1 * u1 * f0->get(i0, j1, k1);
        f32 tu1 = t0 * u0 * f0->get(i1, j0, k0) +
                  t1 * u0 * f0->get(i1, j1, k0) +
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
        f32 comb = (u->get(i + 1, j, k) - u->get(i - 1, j, k)) / m_width +
                   (v->get(i, j + 1, k) - v->get(i, j - 1, k)) / m_width +
                   (w->get(i, j, k + 1) - w->get(i, j, k - 1)) / m_width;
        div->get(i, j, k) = -1.0f / 3.0f * comb;
        prj->get(i, j, k) = 0;
      }
    }
  }

  setBoundary(div, EdgeKind::DENSITY);
  setBoundary(prj, EdgeKind::DENSITY);

  gaussSeidel(prj, div, EdgeKind::DENSITY, 1.0f, 6.0f);

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

  setBoundary(u, EdgeKind::VELOCITY_X);
  setBoundary(v, EdgeKind::VELOCITY_Y);
  setBoundary(w, EdgeKind::VELOCITY_Z);
}

// -------------------------------------------------------------------------- //

void WindSimulation::setBoundary(Field<f32> *f, EdgeKind edge) {
  bool isX = edge == EdgeKind::VELOCITY_X;
  bool isY = edge == EdgeKind::VELOCITY_Y;
  bool isZ = edge == EdgeKind::VELOCITY_Z;

#if 1
  // Obstructions
  for (s32 i = 1; i <= m_width; i++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 k = 1; k <= m_depth; k++) {

        if (i == 23 && j == 12 && k == 16) {
          int y = 0;
        }

        if (m_o->get(i, j, k)) {
          // Summarize negative contributions of neighboring cells
          f32 sum = 0;

          // X dir
          if (!m_o->get(i - 1, j, k)) {
            f32 value = f->get(i - 1, j, k);
            // Debug::rangeCheck(value, -100.0f, 100.0f);
            sum += value;
          }
          if (!m_o->get(i + 1, j, k)) {
            f32 value = f->get(i + 1, j, k);
            // Debug::rangeCheck(value, -100.0f, 100.0f);
            sum += value;
          }

          // Y dir
          if (!m_o->get(i, j - 1, k)) {
            f32 value = f->get(i, j - 1, k);
            // Debug::rangeCheck(value, -100.0f, 100.0f);
            sum += value;
          }
          if (!m_o->get(i, j + 1, k)) {
            f32 value = f->get(i, j + 1, k);
            // Debug::rangeCheck(value, -100.0f, 100.0f);
            sum += value;
          }

          // Y dir
          if (!m_o->get(i, j, k - 1)) {
            f32 value = f->get(i, j, k - 1);
            // Debug::rangeCheck(value, -100.0f, 100.0f);
            sum += value;
          }
          if (!m_o->get(i, j, k + 1)) {
            f32 value = f->get(i, j, k + 1);
            // Debug::rangeCheck(value, -100.0f, 100.0f);
            sum += value;
          }

          f->get(i, j, k) = clamp(-sum, -100.0f, 100.0f);
          // f32 d = f->get(i, j, k);
          // int y = 0;
        }
      }
    }
  }
#endif

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
