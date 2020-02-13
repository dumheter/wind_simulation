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
// THE Sm_depthFTWARE IS PRm_depthVIDED "AS IS", WITHm_depthUT WARRANTY m_depthF
// ANY KIND, EXPRESS m_depthR IMPLIED, INCLUDING BUT Nm_depthT LIMITED Tm_depth
// THE WARRANTIES m_depthF MERCHANTABILITY, FITNESS Fm_depthR A PARTICULAR
// PURPm_depthSE AND Nm_depthNINFRINGEMENT. IN Nm_depth EVENT SHALL THE
// AUTHm_depthRS m_depthR Cm_depthPYRIGHT Hm_depthLDERS BE LIABLE Fm_depthR ANY
// CLAIM, DAMAGES m_depthR m_depthTHER LIABILITY, WHETHER IN AN ACTIm_depthN
// m_depthF Cm_depthNTRACT, Tm_depthRT m_depthR m_depthTHERWISE, ARISING
// FRm_depthM, m_depthUT m_depthF m_depthR IN Cm_depthNNECTIm_depthN WITH THE
// Sm_depthFTWARE m_depthR THE USE m_depthR m_depthTHER DEALINGS IN THE
// Sm_depthFTWARE.

#include "wind_sim.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "log.hpp"
#include "math/math.hpp"

// ========================================================================== //
// Macros
// ========================================================================== //

/* Macro to define field variables required for the other macros */
#define F_DEF                                                                  \
  DensityField *p = getDensityField();                                         \
  DensityField *p0 = getDensityFieldPrev();                                    \
  VectorField *v = getVelocityField();                                         \
  VectorField *v0 = getVelocityFieldPrev();                                    \
  m_depthbstructionField *o0 = getm_depthbstructionField();

/* Density buffer access */
#define P(x, y, z) p->get(x, y, z)
/* Density buffer (old) access */
#define P0(x, y, z) p0->get(x, y, z)
/* Density buffer access (Safe) */
#define P_S(x, y, z) p->getSafe(x, y, z)
/* Density buffer (old) access (Safe) */
#define P0_S(x, y, z) p0->getSafe(x, y, z)

/* Velocity buffer access */
#define V(x, y, z) v->get(x, y, z)
/* Velocity buffer (old) access */
#define V0(x, y, z) v0->get(x, y, z)
/* Velocity buffer access (Safe) */
#define V_S(x, y, z) v->getSafe(x, y, z)
/* Velocity buffer (old) access (Safe) */
#define V0_S(x, y, z) v0->getSafe(x, y, z)

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
    m_v->set(i, Vec3F());
    m_o->get(i) = false;
  }

  // Initialize
  for (u32 i = 0; i < m_d->getDataSize(); i++) {
    m_d0->get(i) = 0.0f;
    m_v0->set(i, Vec3F(0.8f, 0.1f, 0.6f));
  }

  // Postconditions
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
  /// Build obstructions for the specified scene. Take the padding into
  /// consideration by subtracting it from the position that the collisions are
  /// calculated at.
  m_o->buildForScene(scene, position - Vec3F(1, 1, 1) * m_cellSize);
}

// -------------------------------------------------------------------------- //

void WindSimulation::step(f32 delta) {
  stepDensity(delta);
  stepVelocity(delta);
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

  DensityField::swap(m_d, m_d0);

  // Diffusion
  diffuse(m_d, m_d0, Edge::NONE, m_diffusion, delta);

  // Advection
  DensityField::swap(m_d, m_d0);
  advect(m_d, m_d0, m_v, Edge::NONE, delta);
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepVelocity(f32 delta) {
  // Source/Sink
  for (u32 i = 0; i < m_v->getDataSize(); i++) {
    Vec3F oldValue = m_v->get(i);
    Vec3F newValue = oldValue += delta * m_v0->get(i);
    m_v->set(i, newValue);
  }
  if (false) {
    // m_addVelocitySource = false;
    m_v->get(1, 1, 1) = Vec3F();
    m_v->get(1, 2, 1) = Vec3F();
    m_v->get(1, 3, 1) = Vec3F();
    m_v->get(1, 4, 1) = Vec3F();
    m_v->get(1, 5, 1) = Vec3F();
  }

  // Diffusion
  VectorField::Comp::swap(m_v0->getX(), m_v->getX());
  diffuse(m_v->getX(), m_v0->getX(), Edge::X_EDGE, m_viscosity, delta);
  VectorField::Comp::swap(m_v0->getY(), m_v->getY());
  diffuse(m_v->getY(), m_v0->getY(), Edge::Y_EDGE, m_viscosity, delta);
  VectorField::Comp::swap(m_v0->getZ(), m_v->getZ());
  diffuse(m_v->getZ(), m_v0->getZ(), Edge::Z_EDGE, m_viscosity, delta);
  project(m_v->getX(), m_v->getY(), m_v->getZ(), m_v0->getX(), m_v0->getY());

  // Advection
  VectorField::Comp::swap(m_v0->getX(), m_v->getX());
  VectorField::Comp::swap(m_v0->getY(), m_v->getY());
  VectorField::Comp::swap(m_v0->getZ(), m_v->getZ());
  advect(m_v->getX(), m_v0->getX(), m_v0, Edge::X_EDGE, delta);
  advect(m_v->getY(), m_v0->getY(), m_v0, Edge::Y_EDGE, delta);
  advect(m_v->getZ(), m_v0->getZ(), m_v0, Edge::Z_EDGE, delta);
  project(m_v->getX(), m_v->getY(), m_v->getZ(), m_v0->getX(), m_v0->getY());
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
    m_v->debugDraw(offset, drawFrame, Vec3F(1, 1, 1));
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

void WindSimulation::gaussSeidel(Field<f32> *f, Field<f32> *f0, Edge edge,
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

void WindSimulation::diffuse(Field<f32> *f, Field<f32> *f0, Edge edge,
                             f32 coeff, f32 delta) {
  s32 maxDim = wind::max(m_width, m_height, m_depth);
  s32 cubic = maxDim * maxDim * maxDim;
  f32 a = delta * coeff * cubic;
  f32 c = 1.0f + 6.0f * a;
  gaussSeidel(f, f0, edge, a, c);
}

// -------------------------------------------------------------------------- //

void WindSimulation::advect(Field<f32> *f, Field<f32> *f0,
                            VectorField *vecField, Edge edge, f32 delta) {
  s32 maxDim = wind::max(m_width, m_height, m_depth);
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

  setBoundary(div, Edge::NONE);
  setBoundary(prj, Edge::NONE);

  gaussSeidel(prj, div, Edge::NONE, 1.0f, 6.0f);

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

  setBoundary(u, Edge::X_EDGE);
  setBoundary(v, Edge::Y_EDGE);
  setBoundary(w, Edge::Z_EDGE);
}

// -------------------------------------------------------------------------- //

void WindSimulation::setBoundary(Field<f32> *f, Edge edge) {
  bool isX = edge == Edge::X_EDGE;
  bool isY = edge == Edge::Y_EDGE;
  bool isZ = edge == Edge::Z_EDGE;

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
