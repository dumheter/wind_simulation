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
    m_v->get(i) = Vec3F();
    m_o->get(i) = false;
  }

  // Initialize
  for (u32 i = 0; i < m_d->getDataSize(); i++) {
    m_d0->get(i) = 0.0f;
    m_v0->get(i) = Vec3F(0.8f, 0.1f, 0.6f);
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
  DensityField::swap(m_d, m_d0);

  // Diffusion
  stepDensityDiffusion(delta);
  DensityField::swap(m_d, m_d0);

  // Advection
  stepDensityAdvection(delta);
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepVelocity(f32 delta) {
  // Source/Sink
  for (u32 i = 0; i < m_v->getDataSize(); i++) {
    m_v->get(i) += delta * m_v0->get(i);
  }
  if (false) {
    // m_addVelocitySource = false;
    m_v->get(1, 1, 1) = Vec3F();
    m_v->get(1, 2, 1) = Vec3F();
    m_v->get(1, 3, 1) = Vec3F();
    m_v->get(1, 4, 1) = Vec3F();
    m_v->get(1, 5, 1) = Vec3F();
  }
  VectorField::swap(m_v, m_v0);

  // Diffusion
  stepVelocityDiffusion(delta);
  projectVelocity();
  VectorField::swap(m_v, m_v0);

  // Advection
  stepVelocityAdvection(delta);
  projectVelocity();
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

void WindSimulation::stepDensityDiffusion(f32 delta) {
  s32 max = wind::max(m_width, wind::max(m_height, m_depth));
  s32 cubic = max * max * max;
  f32 a = delta * m_diffusion * cubic;
  f32 c = 1.0f + 6.0f * a;

  // lin_solve(M, N, O, b, x, x0, a, 1 + 6 * a);

  // Gauss-Seidel relaxation
  for (s32 l = 0; l < GAUSS_SEIDEL_STEPS; l++) {
    // Diffuse with neighbors
    for (s32 k = 1; k <= m_depth; k++) {
      for (s32 j = 1; j <= m_height; j++) {
        for (s32 i = 1; i <= m_width; i++) {
          f32 in = m_d->get(i - 1, j, k) + m_d->get(i + 1, j, k) +
                   m_d->get(i, j - 1, k) + m_d->get(i, j + 1, k) +
                   m_d->get(i, j, k - 1) + m_d->get(i, j, k + 1);
          m_d->get(i, j, k) = (m_d0->get(i, j, k) + (a * in)) / c;
        }
      }
    }
    setDensityBoundary(m_d);
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepDensityAdvection(f32 delta) {
  f32 dtx = delta * wind::max(m_width, wind::max(m_height, m_depth));
  f32 dty = delta * wind::max(m_width, wind::max(m_height, m_depth));
  f32 dtz = delta * wind::max(m_width, wind::max(m_height, m_depth));

  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 i = 1; i <= m_width; i++) {
        const Vec3F &vec = m_v->get(i, j, k);

        f32 x = clamp(i - dtx * vec.x, 0.5f, m_width + 0.5f);
        s32 i0 = s32(x);
        s32 i1 = i0 + 1;
        f32 s1 = x - i0;
        f32 s0 = 1 - s1;

        f32 y = clamp(j - dty * vec.y, 0.5f, m_height + 0.5f);
        s32 j0 = s32(y);
        s32 j1 = j0 + 1;
        f32 t1 = y - j0;
        f32 t0 = 1 - t1;

        f32 z = clamp(k - dtz * vec.z, 0.5f, m_depth + 0.5f);
        s32 k0 = s32(z);
        s32 k1 = k0 + 1;
        f32 u1 = z - k0;
        f32 u0 = 1 - u1;

        m_d->get(i, j, k) = s0 * (t0 * u0 * m_d0->get(i0, j0, k0) +
                                  t1 * u0 * m_d0->get(i0, j1, k0) +
                                  t0 * u1 * m_d0->get(i0, j0, k1) +
                                  t1 * u1 * m_d0->get(i0, j1, k1)) +
                            s1 * (t0 * u0 * m_d0->get(i1, j0, k0) +
                                  t1 * u0 * m_d0->get(i1, j1, k0) +
                                  t0 * u1 * m_d0->get(i1, j0, k1) +
                                  t1 * u1 * m_d0->get(i1, j1, k1));
      }
    }
  }

  setDensityBoundary(m_d);
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepVelocityDiffusion(f32 delta) {
  s32 max = wind::max(m_width, wind::max(m_height, m_depth));
  s32 cubic = max * max * max;
  f32 a = delta * m_viscosity * cubic;
  f32 c = 1.0f + 6.0f * a;

  // Gauss-Seidel relaxation
  for (s32 l = 0; l < GAUSS_SEIDEL_STEPS; l++) {
    // Diffuse with neighbors
    for (s32 k = 1; k <= m_depth; k++) {
      for (s32 j = 1; j <= m_height; j++) {
        for (s32 i = 1; i <= m_width; i++) {
          Vec3F in = m_v->get(i - 1, j, k) + m_v->get(i + 1, j, k) +
                     m_v->get(i, j - 1, k) + m_v->get(i, j + 1, k) +
                     m_v->get(i, j, k - 1) + m_v->get(i, j, k + 1);
          m_v->get(i, j, k) = (m_v0->get(i, j, k) + (a * in)) / c;
        }
      }
    }
    setVelocityBoundary(m_v);
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepVelocityAdvection(f32 delta) {
  f32 dtx = delta * wind::max(m_width, wind::max(m_height, m_depth));
  f32 dty = delta * wind::max(m_width, wind::max(m_height, m_depth));
  f32 dtz = delta * wind::max(m_width, wind::max(m_height, m_depth));

  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 i = 1; i <= m_width; i++) {
        const Vec3F &vec = m_v->get(i, j, k);

        f32 x = clamp(i - dtx * vec.x, 0.5f, m_width + 0.5f);
        s32 i0 = s32(x);
        s32 i1 = i0 + 1;
        f32 s1 = x - i0;
        f32 s0 = 1 - s1;

        f32 y = clamp(j - dty * vec.y, 0.5f, m_height + 0.5f);
        s32 j0 = s32(y);
        s32 j1 = j0 + 1;
        f32 t1 = y - j0;
        f32 t0 = 1 - t1;

        f32 z = clamp(k - dtz * vec.z, 0.5f, m_depth + 0.5f);
        s32 k0 = s32(z);
        s32 k1 = k0 + 1;
        f32 u1 = z - k0;
        f32 u0 = 1 - u1;

        m_v->get(i, j, k) = s0 * (t0 * u0 * m_v0->get(i0, j0, k0) +
                                  t1 * u0 * m_v0->get(i0, j1, k0) +
                                  t0 * u1 * m_v0->get(i0, j0, k1) +
                                  t1 * u1 * m_v0->get(i0, j1, k1)) +
                            s1 * (t0 * u0 * m_v0->get(i1, j0, k0) +
                                  t1 * u0 * m_v0->get(i1, j1, k0) +
                                  t0 * u1 * m_v0->get(i1, j0, k1) +
                                  t1 * u1 * m_v0->get(i1, j1, k1));
      }
    }
  }

  setVelocityBoundary(m_v);
}

// -------------------------------------------------------------------------- //

void WindSimulation::projectVelocity() {
  /// p = (m_v0[N].x)
  /// div = (m_v0[N].y)

  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 i = 1; i <= m_width; i++) {
        f32 val_x0 = m_v->get(i + 1, j, k).x;
        f32 val_x1 = m_v->get(i - 1, j, k).x;
        f32 val_y0 = m_v->get(i, j + 1, k).y;
        f32 val_y1 = m_v->get(i, j - 1, k).y;
        f32 val_z0 = m_v->get(i, j, k + 1).z;
        f32 val_z1 = m_v->get(i, j, k - 1).z;
        f32 other = (val_x0 - val_x1) / m_width + (val_y0 - val_y1) / m_width +
                    (val_z0 - val_z1) / m_width;
        m_v0->get(i, j, k).y = -1.0f / 3.0f * other;
        m_v0->get(i, j, k).x = 0.0f;
        ;
      }
    }
  }

  setProjectBoundaryY(m_v0);
  setProjectBoundaryX(m_v0);

  // Gauss-Seidel relaxation
  for (s32 l = 0; l < GAUSS_SEIDEL_STEPS; l++) {
    // Diffuse with neighbors
    for (s32 k = 1; k <= m_depth; k++) {
      for (s32 j = 1; j <= m_height; j++) {
        for (s32 i = 1; i <= m_width; i++) {
          f32 comb = (m_v0->get(i - 1, j, k).x + m_v0->get(i + 1, j, k).x +
                      m_v0->get(i, j - 1, k).x + m_v0->get(i, j + 1, k).x +
                      m_v0->get(i, j, k - 1).x + m_v0->get(i, j, k + 1).x);
          m_v0->get(i, j, k).x = (m_v0->get(i, j, k).y + 1.0f * comb) / 6.0f;
        }
      }
    }
    setProjectBoundaryX(m_v0);
  }

  for (s32 i = 1; i <= m_width; i++) {
    for (s32 j = 1; j <= m_height; j++) {
      for (s32 k = 1; k <= m_depth; k++) {
        m_v->get(i, j, k).x -=
            0.5f * m_width *
            (m_v0->get(i + 1, j, k).x - m_v0->get(i - 1, j, k).x);
        m_v->get(i, j, k).y -=
            0.5f * m_width *
            (m_v0->get(i, j + 1, k).x - m_v0->get(i, j - 1, k).x);
        m_v->get(i, j, k).z -=
            0.5f * m_width *
            (m_v0->get(i, j, k + 1).x - m_v0->get(i, j, k - 1).x);
      }
    }
  }

  setVelocityBoundary(m_v);
}

// -------------------------------------------------------------------------- //

void WindSimulation::setDensityBoundary(DensityField *field) {
  // X-Y face
  for (s32 j = 1; j <= m_height; j++) {
    for (s32 i = 1; i <= m_width; i++) {
      field->get(i, j, 0) = field->get(i, j, 1);
      field->get(i, j, m_depth + 1) = field->get(i, j, m_depth);
    }
  }

  // Y-Z face
  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      field->get(0, j, k) = field->get(1, j, k);
      field->get(m_width + 1, j, k) = field->get(m_width, j, k);
    }
  }

  // X-Z face
  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 i = 1; i <= m_width; i++) {
      field->get(i, 0, k) = field->get(i, 1, k);
      field->get(i, m_height + 1, k) = field->get(i, m_height, k);
    }
  }

  // X edge
  for (s32 i = 1; i <= m_width; i++) {
    field->get(i, 0, 0) =
        1.0f / 2.0f * (field->get(i, 1, 0) + field->get(i, 0, 1));
    field->get(i, m_height + 1, 0) =
        1.0f / 2.0f *
        (field->get(i, m_height, 0) + field->get(i, m_height + 1, 1));
    field->get(i, 0, m_depth + 1) =
        1.0f / 2.0f *
        (field->get(i, 0, m_depth) + field->get(i, 1, m_depth + 1));
    field->get(i, m_height + 1, m_depth + 1) =
        1.0f / 2.0f *
        (field->get(i, m_height, m_depth + 1) +
         field->get(i, m_height + 1, m_depth));
  }

  // Y edge
  for (s32 i = 1; i <= m_height; i++) {
    field->get(0, i, 0) =
        1.0f / 2.0f * (field->get(1, i, 0) + field->get(0, i, 1));
    field->get(m_width + 1, i, 0) =
        1.0f / 2.0f *
        (field->get(m_width, i, 0) + field->get(m_width + 1, i, 1));
    field->get(0, i, m_depth + 1) =
        1.0f / 2.0f *
        (field->get(0, i, m_depth) + field->get(1, i, m_depth + 1));
    field->get(m_width + 1, i, m_depth + 1) =
        1.0f / 2.0f *
        (field->get(m_width, i, m_depth + 1) +
         field->get(m_width + 1, i, m_depth));
  }

  // Z edge
  for (s32 i = 1; i <= m_depth; i++) {
    field->get(0, 0, i) =
        1.0f / 2.0f * (field->get(0, 1, i) + field->get(1, 0, i));
    field->get(0, m_height + 1, i) =
        1.0f / 2.0f *
        (field->get(0, m_height, i) + field->get(1, m_height + 1, i));
    field->get(m_width + 1, 0, i) =
        1.0f / 2.0f *
        (field->get(m_width, 0, i) + field->get(m_width + 1, 1, i));
    field->get(m_width + 1, m_height + 1, i) =
        1.0f / 2.0f *
        (field->get(m_width + 1, m_height, i) +
         field->get(m_width, m_height + 1, i));
  }

  // Corners
  field->get(0, 0, 0) =
      1.0f / 3.0f *
      (field->get(1, 0, 0) + field->get(0, 1, 0) + field->get(0, 0, 1));
  field->get(0, m_height + 1, 0) =
      1.0f / 3.0f *
      (field->get(1, m_height + 1, 0) + field->get(0, m_height, 0) +
       field->get(0, m_height + 1, 1));
  field->get(m_width + 1, 0, 0) =
      1.0f / 3.0f *
      (field->get(m_width, 0, 0) + field->get(m_width + 1, 1, 0) +
       field->get(m_width + 1, 0, 1));
  field->get(m_width + 1, m_height + 1, 0) =
      1.0f / 3.0f *
      (field->get(m_width, m_height + 1, 0) +
       field->get(m_width + 1, m_height, 0) +
       field->get(m_width + 1, m_height + 1, 1));
  field->get(0, 0, m_depth + 1) =
      1.0f / 3.0f *
      (field->get(1, 0, m_depth + 1) + field->get(0, 1, m_depth + 1) +
       field->get(0, 0, m_depth));
  field->get(0, m_height + 1, m_depth + 1) =
      1.0f / 3.0f *
      (field->get(1, m_height + 1, m_depth + 1) +
       field->get(0, m_height, m_depth + 1) +
       field->get(0, m_height + 1, m_depth));
  field->get(m_width + 1, 0, m_depth + 1) =
      1.0f / 3.0f *
      (field->get(m_width, 0, m_depth + 1) +
       field->get(m_width + 1, 1, m_depth + 1) +
       field->get(m_width + 1, 0, m_depth));
  field->get(m_width + 1, m_height + 1, m_depth + 1) =
      1.0f / 3.0f *
      (field->get(m_width, m_height + 1, m_depth + 1) +
       field->get(m_width + 1, m_height, m_depth + 1) +
       field->get(m_width + 1, m_height + 1, m_depth));
}

// -------------------------------------------------------------------------- //

void WindSimulation::setVelocityBoundary(VectorField *field) {
  // X -> b = 1
  // Y -> b = 2
  // Z -> b = 3

  // X-Y face
  for (s32 j = 1; j <= m_height; j++) {
    for (s32 i = 1; i <= m_width; i++) {
      Vec3F v_a = field->get(i, j, 1);
      Vec3F v_b = field->get(i, j, m_depth);
      field->get(i, j, 0) = Vec3F(v_a.x, v_a.y, -v_a.z);
      field->get(i, j, m_depth) = Vec3F(v_b.x, v_b.y, -v_b.z);
    }
  }

  // Y-Z face
  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      Vec3F v_a = field->get(1, j, k);
      Vec3F v_b = field->get(m_width, j, k);
      field->get(0, j, k) = Vec3F(-v_a.x, v_a.y, v_a.z);
      field->get(m_width, j, k) = Vec3F(-v_b.x, v_b.y, v_b.z);
    }
  }

  // X-Z face
  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 i = 1; i <= m_width; i++) {
      Vec3F v_a = field->get(i, 1, k);
      Vec3F v_b = field->get(i, m_height, k);
      field->get(i, 0, k) = Vec3F(v_a.x, -v_a.y, v_a.z);
      field->get(i, m_height, k) = Vec3F(v_b.x, -v_b.y, v_b.z);
    }
  }

  // X edge
  for (s32 i = 1; i <= m_width; i++) {
    field->get(i, 0, 0) =
        1.0f / 2.0f * (field->get(i, 1, 0) + field->get(i, 0, 1));
    field->get(i, m_height + 1, 0) =
        1.0f / 2.0f *
        (field->get(i, m_height, 0) + field->get(i, m_height + 1, 1));
    field->get(i, 0, m_depth + 1) =
        1.0f / 2.0f *
        (field->get(i, 0, m_depth) + field->get(i, 1, m_depth + 1));
    field->get(i, m_height + 1, m_depth + 1) =
        1.0f / 2.0f *
        (field->get(i, m_height, m_depth + 1) +
         field->get(i, m_height + 1, m_depth));
  }

  // Y edge
  for (s32 i = 1; i <= m_height; i++) {
    field->get(0, i, 0) =
        1.0f / 2.0f * (field->get(1, i, 0) + field->get(0, i, 1));
    field->get(m_width + 1, i, 0) =
        1.0f / 2.0f *
        (field->get(m_width, i, 0) + field->get(m_width + 1, i, 1));
    field->get(0, i, m_depth + 1) =
        1.0f / 2.0f *
        (field->get(0, i, m_depth) + field->get(1, i, m_depth + 1));
    field->get(m_width + 1, i, m_depth + 1) =
        1.0f / 2.0f *
        (field->get(m_width, i, m_depth + 1) +
         field->get(m_width + 1, i, m_depth));
  }

  // Z edge
  for (s32 i = 1; i <= m_depth; i++) {
    field->get(0, 0, i) =
        1.0f / 2.0f * (field->get(0, 1, i) + field->get(1, 0, i));
    field->get(0, m_height + 1, i) =
        1.0f / 2.0f *
        (field->get(0, m_height, i) + field->get(1, m_height + 1, i));
    field->get(m_width + 1, 0, i) =
        1.0f / 2.0f *
        (field->get(m_width, 0, i) + field->get(m_width + 1, 1, i));
    field->get(m_width + 1, m_height + 1, i) =
        1.0f / 2.0f *
        (field->get(m_width + 1, m_height, i) +
         field->get(m_width, m_height + 1, i));
  }

  // Corners
  field->get(0, 0, 0) =
      1.0f / 3.0f *
      (field->get(1, 0, 0) + field->get(0, 1, 0) + field->get(0, 0, 1));
  field->get(0, m_height + 1, 0) =
      1.0f / 3.0f *
      (field->get(1, m_height + 1, 0) + field->get(0, m_height, 0) +
       field->get(0, m_height + 1, 1));
  field->get(m_width + 1, 0, 0) =
      1.0f / 3.0f *
      (field->get(m_width, 0, 0) + field->get(m_width + 1, 1, 0) +
       field->get(m_width + 1, 0, 1));
  field->get(m_width + 1, m_height + 1, 0) =
      1.0f / 3.0f *
      (field->get(m_width, m_height + 1, 0) +
       field->get(m_width + 1, m_height, 0) +
       field->get(m_width + 1, m_height + 1, 1));
  field->get(0, 0, m_depth + 1) =
      1.0f / 3.0f *
      (field->get(1, 0, m_depth + 1) + field->get(0, 1, m_depth + 1) +
       field->get(0, 0, m_depth));
  field->get(0, m_height + 1, m_depth + 1) =
      1.0f / 3.0f *
      (field->get(1, m_height + 1, m_depth + 1) +
       field->get(0, m_height, m_depth + 1) +
       field->get(0, m_height + 1, m_depth));
  field->get(m_width + 1, 0, m_depth + 1) =
      1.0f / 3.0f *
      (field->get(m_width, 0, m_depth + 1) +
       field->get(m_width + 1, 1, m_depth + 1) +
       field->get(m_width + 1, 0, m_depth));
  field->get(m_width + 1, m_height + 1, m_depth + 1) =
      1.0f / 3.0f *
      (field->get(m_width, m_height + 1, m_depth + 1) +
       field->get(m_width + 1, m_height, m_depth + 1) +
       field->get(m_width + 1, m_height + 1, m_depth));
}

// -------------------------------------------------------------------------- //

void WindSimulation::setProjectBoundaryX(VectorField *field) {
  // X-Y face
  for (s32 j = 1; j <= m_height; j++) {
    for (s32 i = 1; i <= m_width; i++) {
      field->get(i, j, 0).x = field->get(i, j, 1).x;
      field->get(i, j, m_depth + 1).x = field->get(i, j, m_depth).x;
    }
  }

  // Y-Z face
  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      field->get(0, j, k).x = field->get(1, j, k).x;
      field->get(m_width + 1, j, k).x = field->get(m_width, j, k).x;
    }
  }

  // X-Z face
  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 i = 1; i <= m_width; i++) {
      field->get(i, 0, k).x = field->get(i, 1, k).x;
      field->get(i, m_height + 1, k).x = field->get(i, m_height, k).x;
    }
  }

  // X edge
  for (s32 i = 1; i <= m_width; i++) {
    field->get(i, 0, 0).x =
        1.0f / 2.0f * (field->get(i, 1, 0).x + field->get(i, 0, 1).x);
    field->get(i, m_height + 1, 0).x =
        1.0f / 2.0f *
        (field->get(i, m_height, 0).x + field->get(i, m_height + 1, 1).x);
    field->get(i, 0, m_depth + 1).x =
        1.0f / 2.0f *
        (field->get(i, 0, m_depth).x + field->get(i, 1, m_depth + 1).x);
    field->get(i, m_height + 1, m_depth + 1).x =
        1.0f / 2.0f *
        (field->get(i, m_height, m_depth + 1).x +
         field->get(i, m_height + 1, m_depth).x);
  }

  // Y edge
  for (s32 i = 1; i <= m_height; i++) {
    field->get(0, i, 0).x =
        1.0f / 2.0f * (field->get(1, i, 0).x + field->get(0, i, 1).x);
    field->get(m_width + 1, i, 0) =
        1.0f / 2.0f *
        (field->get(m_width, i, 0).x + field->get(m_width + 1, i, 1).x);
    field->get(0, i, m_depth + 1).x =
        1.0f / 2.0f *
        (field->get(0, i, m_depth).x + field->get(1, i, m_depth + 1).x);
    field->get(m_width + 1, i, m_depth + 1).x =
        1.0f / 2.0f *
        (field->get(m_width, i, m_depth + 1).x +
         field->get(m_width + 1, i, m_depth).x);
  }

  // Z edge
  for (s32 i = 1; i <= m_depth; i++) {
    field->get(0, 0, i).x =
        1.0f / 2.0f * (field->get(0, 1, i).x + field->get(1, 0, i).x);
    field->get(0, m_height + 1, i).x =
        1.0f / 2.0f *
        (field->get(0, m_height, i).x + field->get(1, m_height + 1, i).x);
    field->get(m_width + 1, 0, i).x =
        1.0f / 2.0f *
        (field->get(m_width, 0, i).x + field->get(m_width + 1, 1, i).x);
    field->get(m_width + 1, m_height + 1, i).x =
        1.0f / 2.0f *
        (field->get(m_width + 1, m_height, i).x +
         field->get(m_width, m_height + 1, i).x);
  }

  // Corners
  field->get(0, 0, 0).x =
      1.0f / 3.0f *
      (field->get(1, 0, 0).x + field->get(0, 1, 0).x + field->get(0, 0, 1).x);
  field->get(0, m_height + 1, 0).x =
      1.0f / 3.0f *
      (field->get(1, m_height + 1, 0).x + field->get(0, m_height, 0).x +
       field->get(0, m_height + 1, 1).x);
  field->get(m_width + 1, 0, 0).x =
      1.0f / 3.0f *
      (field->get(m_width, 0, 0).x + field->get(m_width + 1, 1, 0).x +
       field->get(m_width + 1, 0, 1).x);
  field->get(m_width + 1, m_height + 1, 0).x =
      1.0f / 3.0f *
      (field->get(m_width, m_height + 1, 0).x +
       field->get(m_width + 1, m_height, 0).x +
       field->get(m_width + 1, m_height + 1, 1).x);
  field->get(0, 0, m_depth + 1).x =
      1.0f / 3.0f *
      (field->get(1, 0, m_depth + 1).x + field->get(0, 1, m_depth + 1).x +
       field->get(0, 0, m_depth).x);
  field->get(0, m_height + 1, m_depth + 1).x =
      1.0f / 3.0f *
      (field->get(1, m_height + 1, m_depth + 1).x +
       field->get(0, m_height, m_depth + 1).x +
       field->get(0, m_height + 1, m_depth).x);
  field->get(m_width + 1, 0, m_depth + 1).x =
      1.0f / 3.0f *
      (field->get(m_width, 0, m_depth + 1).x +
       field->get(m_width + 1, 1, m_depth + 1).x +
       field->get(m_width + 1, 0, m_depth).x);
  field->get(m_width + 1, m_height + 1, m_depth + 1).x =
      1.0f / 3.0f *
      (field->get(m_width, m_height + 1, m_depth + 1).x +
       field->get(m_width + 1, m_height, m_depth + 1).x +
       field->get(m_width + 1, m_height + 1, m_depth).x);
}

// -------------------------------------------------------------------------- //

void WindSimulation::setProjectBoundaryY(VectorField *field) {
  // X-Y face
  for (s32 j = 1; j <= m_height; j++) {
    for (s32 i = 1; i <= m_width; i++) {
      field->get(i, j, 0).y = field->get(i, j, 1).y;
      field->get(i, j, m_depth + 1).y = field->get(i, j, m_depth).y;
    }
  }

  // Y-Z face
  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 j = 1; j <= m_height; j++) {
      field->get(0, j, k).y = field->get(1, j, k).y;
      field->get(m_width + 1, j, k).y = field->get(m_width, j, k).y;
    }
  }

  // X-Z face
  for (s32 k = 1; k <= m_depth; k++) {
    for (s32 i = 1; i <= m_width; i++) {
      field->get(i, 0, k).y = field->get(i, 1, k).y;
      field->get(i, m_height + 1, k).y = field->get(i, m_height, k).y;
    }
  }

  // X edge
  for (s32 i = 1; i <= m_width; i++) {
    field->get(i, 0, 0).y =
        1.0f / 2.0f * (field->get(i, 1, 0).y + field->get(i, 0, 1).y);
    field->get(i, m_height + 1, 0).y =
        1.0f / 2.0f *
        (field->get(i, m_height, 0).y + field->get(i, m_height + 1, 1).y);
    field->get(i, 0, m_depth + 1).y =
        1.0f / 2.0f *
        (field->get(i, 0, m_depth).y + field->get(i, 1, m_depth + 1).y);
    field->get(i, m_height + 1, m_depth + 1).y =
        1.0f / 2.0f *
        (field->get(i, m_height, m_depth + 1).y +
         field->get(i, m_height + 1, m_depth).y);
  }

  // Y edge
  for (s32 i = 1; i <= m_height; i++) {
    field->get(0, i, 0).y =
        1.0f / 2.0f * (field->get(1, i, 0).y + field->get(0, i, 1).y);
    field->get(m_width + 1, i, 0) =
        1.0f / 2.0f *
        (field->get(m_width, i, 0).y + field->get(m_width + 1, i, 1).y);
    field->get(0, i, m_depth + 1).y =
        1.0f / 2.0f *
        (field->get(0, i, m_depth).y + field->get(1, i, m_depth + 1).y);
    field->get(m_width + 1, i, m_depth + 1).y =
        1.0f / 2.0f *
        (field->get(m_width, i, m_depth + 1).y +
         field->get(m_width + 1, i, m_depth).y);
  }

  // Z edge
  for (s32 i = 1; i <= m_depth; i++) {
    field->get(0, 0, i).y =
        1.0f / 2.0f * (field->get(0, 1, i).y + field->get(1, 0, i).y);
    field->get(0, m_height + 1, i).y =
        1.0f / 2.0f *
        (field->get(0, m_height, i).y + field->get(1, m_height + 1, i).y);
    field->get(m_width + 1, 0, i).y =
        1.0f / 2.0f *
        (field->get(m_width, 0, i).y + field->get(m_width + 1, 1, i).y);
    field->get(m_width + 1, m_height + 1, i).y =
        1.0f / 2.0f *
        (field->get(m_width + 1, m_height, i).y +
         field->get(m_width, m_height + 1, i).y);
  }

  // Corners
  field->get(0, 0, 0).y =
      1.0f / 3.0f *
      (field->get(1, 0, 0).y + field->get(0, 1, 0).y + field->get(0, 0, 1).y);
  field->get(0, m_height + 1, 0).y =
      1.0f / 3.0f *
      (field->get(1, m_height + 1, 0).y + field->get(0, m_height, 0).y +
       field->get(0, m_height + 1, 1).y);
  field->get(m_width + 1, 0, 0).y =
      1.0f / 3.0f *
      (field->get(m_width, 0, 0).y + field->get(m_width + 1, 1, 0).y +
       field->get(m_width + 1, 0, 1).y);
  field->get(m_width + 1, m_height + 1, 0).y =
      1.0f / 3.0f *
      (field->get(m_width, m_height + 1, 0).y +
       field->get(m_width + 1, m_height, 0).y +
       field->get(m_width + 1, m_height + 1, 1).y);
  field->get(0, 0, m_depth + 1).y =
      1.0f / 3.0f *
      (field->get(1, 0, m_depth + 1).y + field->get(0, 1, m_depth + 1).y +
       field->get(0, 0, m_depth).y);
  field->get(0, m_height + 1, m_depth + 1).y =
      1.0f / 3.0f *
      (field->get(1, m_height + 1, m_depth + 1).y +
       field->get(0, m_height, m_depth + 1).y +
       field->get(0, m_height + 1, m_depth).y);
  field->get(m_width + 1, 0, m_depth + 1).y =
      1.0f / 3.0f *
      (field->get(m_width, 0, m_depth + 1).y +
       field->get(m_width + 1, 1, m_depth + 1).y +
       field->get(m_width + 1, 0, m_depth).y);
  field->get(m_width + 1, m_height + 1, m_depth + 1).y =
      1.0f / 3.0f *
      (field->get(m_width, m_height + 1, m_depth + 1).y +
       field->get(m_width + 1, m_height, m_depth + 1).y +
       field->get(m_width + 1, m_height + 1, m_depth).y);
}

} // namespace wind
