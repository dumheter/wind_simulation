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
  ObstructionField *o0 = getObstructionField();

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

WindSimulation::~WindSimulation() {
  for (u32 i = 0; i < BUFFER_COUNT; i++) {
    delete m_densityFields[i];
    delete m_velocityFields[i];
  }
  delete m_obstructionField;
}

// -------------------------------------------------------------------------- //

void WindSimulation::step(f32 delta) {
  stepDensity(delta);
  stepVelocity(delta);
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepDensity(f32 delta) {
  F_DEF

  // Sources and sinks
  if (m_addDensitySource || m_addDensitySink) {
    if (m_addDensitySource) {
      m_addDensitySource = false;

      P(0, 0, 0) = 1.0f;
      P(0, 1, 0) = 1.0f;
      P(0, 2, 0) = 1.0f;
      P(0, 3, 0) = 1.0f;
      P(0, 4, 0) = 1.0f;
    }
    if (m_addDensitySink) {
      m_addDensitySink = false;

      // P(m_width - 1, 0, m_depth - 1) = 0.0f;
      // P(m_width - 1, 1, m_depth - 1) = 0.0f;
      // P(m_width - 1, 2, m_depth - 1) = 0.0f;
      // P(m_width - 1, 3, m_depth - 1) = 0.0f;
      // P(m_width - 1, 4, m_depth - 1) = 0.0f;
      // P(m_width - 1, 5, m_depth - 1) = 0.0f;
      //
      // P(m_width - 2, 0, m_depth - 2) = 0.0f;
      // P(m_width - 2, 1, m_depth - 2) = 0.0f;
      // P(m_width - 2, 2, m_depth - 2) = 0.0f;
      // P(m_width - 2, 3, m_depth - 2) = 0.0f;
      // P(m_width - 2, 4, m_depth - 2) = 0.0f;
      // P(m_width - 2, 5, m_depth - 2) = 0.0f;

      for (s32 j = 0; j < s32(m_height); j++) {
        for (s32 i = 0; i < s32(m_width); i++) {
          P(i, j, m_depth - 1) = 0.0f;
        }
      }
    }

    updateDensityBufferIndex();
  }

  // Diffusion
  if (m_densityDiffusionActive) {
    stepDensityDiffusion(delta);
    updateDensityBufferIndex();
  }

  // Advection
  if (m_densityAdvectionActive) {
    stepDensityAdvection(delta);
  }

  // 1. 6.  11. 14
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepVelocity(f32 delta) {
  // Sources and sinks

  // Diffusion
  if (m_velocityDiffusionActive) {
    stepVelocityDiffusion(delta);
    updateVelocityBufferIndex();
  }

  // Projection
  projectVelocity();

  // Advection
  if (m_velocityAdvectionActive) {
    stepVelocityAdvection(delta);
  }

  // Projection
  projectVelocity();
}

// -------------------------------------------------------------------------- //

void WindSimulation::init() {
  // Initialize density fields
  for (u32 i = 0; i < getDensityField()->getDataSize(); i++) {
    DensityField::Pos pos = getDensityField()->fromOffset(i);
    for (u32 j = 0; j < BUFFER_COUNT; j++) {
      m_densityFields[j]->get(i) = 0.0f;
    }
  }

  // Initialize the vector fields
  for (u32 j = 0; j < BUFFER_COUNT; j++) {
    m_velocityFields[j]->setBorder(VectorField::BorderKind::CONTAINED);
    for (u32 i = 0; i < getVelocityField()->getDataSize(); i++) {
      VectorField::Pos pos = getVelocityField()->fromOffset(i);
      m_velocityFields[j]->get(i) = bs::Vector3(0.8f, 0.1f, 0.6f) * 5.0f;
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepDensityDiffusion(f32 delta) {
  F_DEF

  f32 maxDim = f32(
      wind::max(wind::max(m_width, m_height), wind::max(m_height, m_depth)));
  f32 cubic = std::pow(maxDim, 3);
  f32 a = delta * m_diffusion * cubic;
  f32 c = 1.0f + 6.0f * a;

  // Gauss-Seidel relaxation (GAUSS_SEIDEL_STEPS number of steps)
  for (u32 _gs = 0; _gs < GAUSS_SEIDEL_STEPS; _gs++) {
    // Diffuse with neighbors
    for (u32 k = 0; k < m_depth; k++) {
      for (u32 j = 0; j < m_height; j++) {
        for (u32 i = 0; i < m_width; i++) {
          f32 adj = P_S(i - 1, j, k) + P_S(i + 1, j, k) + P_S(i, j - 1, k) +
                    P_S(i, j + 1, k) + P_S(i, j, k - 1) + P_S(i, j, k + 1);
          P(i, j, k) = (P0(i, j, k) + a * adj) / c;
        }
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepDensityAdvection(f32 delta) {
  F_DEF

  f32 maxDim = f32(
      wind::max(wind::max(m_width, m_height), wind::max(m_height, m_depth)));
  f32 dty = delta * maxDim;
  f32 dtz = delta * maxDim;
  f32 dtx = delta * maxDim;

  for (s32 k = 0; k < s32(m_depth); k++) {
    for (s32 j = 0; j < s32(m_height); j++) {
      for (s32 i = 0; i < s32(m_width); i++) {
        // Calculate interpolation points (a0, a1), (b0, b1) and (c0, c1) by
        // following the wind vector backwards from the cell, seeing where the
        // density has flowed from.
        bs::Vector3 &vel = V(i, j, k);

        f32 clampLower = 0.5f;
        f32 clampUpper = 0.5f;

        f32 iPrev = clamp(i - dtx * vel.x, clampLower, m_width + clampUpper);
        s32 i0 = s32(std::floor(iPrev));
        s32 i1 = i0 + 1;
        f32 a1 = iPrev - i0, a0 = 1 - a1;

        f32 jPrev = clamp(j - dty * vel.y, clampLower, m_height + clampUpper);
        s32 j0 = s32(std::floor(jPrev));
        s32 j1 = j0 + 1;
        f32 b1 = jPrev - j0, b0 = 1 - b1;

        f32 kPrev = clamp(k - dtz * vel.z, clampLower, m_depth + clampUpper);
        s32 k0 = s32(std::floor(kPrev));
        s32 k1 = k0 + 1;
        f32 c1 = kPrev - k0, c0 = 1 - c1;

        // Interpolate the densities
        P(i, j, k) =
            a0 * (b0 * c0 * P0_S(i0, j0, k0) + b1 * c0 * P0_S(i0, j1, k0) +
                  b0 * c1 * P0_S(i0, j0, k1) + b1 * c1 * P0_S(i0, j1, k1)) +
            a1 * (b0 * c0 * P0_S(i1, j0, k0) + b1 * c0 * P0_S(i1, j1, k0) +
                  b0 * c1 * P0_S(i1, j0, k1) + b1 * c1 * P0_S(i1, j1, k1));
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepVelocityDiffusion(f32 delta) {
  F_DEF

  f32 maxDim = f32(
      wind::max(wind::max(m_width, m_height), wind::max(m_height, m_depth)));
  f32 cubic = std::pow(maxDim, 3);
  f32 a = delta * m_viscosity * cubic;
  f32 c = 1.0f + 6.0f * a;

  // Gauss-Seidel relaxation (GAUSS_SEIDEL_STEPS number of steps)
  for (u32 _gs = 0; _gs < GAUSS_SEIDEL_STEPS; _gs++) {
    // Diffuse with neighbors
    for (u32 k = 0; k < m_depth; k++) {
      for (u32 j = 0; j < m_height; j++) {
        for (u32 i = 0; i < m_width; i++) {
          bs::Vector3 adj = V_S(i - 1, j, k) + V_S(i + 1, j, k) +
                            V_S(i, j - 1, k) + V_S(i, j + 1, k) +
                            V_S(i, j, k - 1) + V_S(i, j, k + 1);
          V(i, j, k) = (V0(i, j, k) + a * adj) / c;
        }
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepVelocityAdvection(f32 delta) {
  F_DEF

  f32 maxDim = f32(
      wind::max(wind::max(m_width, m_height), wind::max(m_height, m_depth)));
  f32 dty = delta * maxDim;
  f32 dtz = delta * maxDim;
  f32 dtx = delta * maxDim;

  for (s32 k = 0; k < s32(m_depth); k++) {
    for (s32 j = 0; j < s32(m_height); j++) {
      for (s32 i = 0; i < s32(m_width); i++) {
        // Calculate interpolation points (a0, a1), (b0, b1) and (c0, c1) by
        // following the wind vector backwards from the cell, seeing where the
        // density has flowed from.
        bs::Vector3 &vel = V(i, j, k);

        f32 clampLower = 0.5f;
        f32 clampUpper = 0.5f;

        f32 iPrev = clamp(i - dtx * vel.x, clampLower, m_width + clampUpper);
        s32 i0 = s32(std::floor(iPrev));
        s32 i1 = i0 + 1;
        f32 a1 = iPrev - i0, a0 = 1 - a1;

        f32 jPrev = clamp(j - dty * vel.y, clampLower, m_height + clampUpper);
        s32 j0 = s32(std::floor(jPrev));
        s32 j1 = j0 + 1;
        f32 b1 = jPrev - j0, b0 = 1 - b1;

        f32 kPrev = clamp(k - dtz * vel.z, clampLower, m_depth + clampUpper);
        s32 k0 = s32(std::floor(kPrev));
        s32 k1 = k0 + 1;
        f32 c1 = kPrev - k0, c0 = 1 - c1;

        // Interpolate the densities
        V(i, j, k) =
            a0 * (b0 * c0 * V0_S(i0, j0, k0) + b1 * c0 * V0_S(i0, j1, k0) +
                  b0 * c1 * V0_S(i0, j0, k1) + b1 * c1 * V0_S(i0, j1, k1)) +
            a1 * (b0 * c0 * V0_S(i1, j0, k0) + b1 * c0 * V0_S(i1, j1, k0) +
                  b0 * c1 * V0_S(i1, j0, k1) + b1 * c1 * V0_S(i1, j1, k1));
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::projectVelocity() {
  F_DEF

  // Step 1
  for (s32 k = 0; k < s32(m_depth); k++) {
    for (s32 j = 0; j < s32(m_height); j++) {
      for (s32 i = 0; i < s32(m_width); i++) {
        f32 mul = (V_S(i + 1, j, k).x - V_S(i - 1, j, k).x) / m_width +
                  (V_S(i, j + 1, k).y - V_S(i, j - 1, k).y) / m_height +
                  (V_S(i, j, k + 1).z - V_S(i, j, k - 1).z) / m_depth;
        V0(i, j, k).y = -1.0f / 3.0f * mul;
        V0(i, j, k).x = 0.0f;
      }
    }
  }

  v0->setBorder(VectorField::BorderKind::EDGE);

  // Gauss-Seidel relaxation (GAUSS_SEIDEL_STEPS number of steps)
  for (u32 _gs = 0; _gs < GAUSS_SEIDEL_STEPS; _gs++) {
    // Diffuse with neighbors
    for (u32 k = 0; k < m_depth; k++) {
      for (u32 j = 0; j < m_height; j++) {
        for (u32 i = 0; i < m_width; i++) {
          f32 value =
              (V0_S(i - 1, j, k).x + V0_S(i + 1, j, k).x + V0_S(i, j - 1, k).x +
               V0_S(i, j + 1, k).x + V0_S(i, j, k - 1).x + V0_S(i, j, k + 1).x);
          V0(i, j, k).x = (V0(i, j, k).y + 1.0f * value) / 6.0f;
        }
      }
    }
  }

  // Step 2
  for (s32 k = 0; k < s32(m_depth); k++) {
    for (s32 j = 0; j < s32(m_height); j++) {
      for (s32 i = 0; i < s32(m_width); i++) {
        V(i, j, k).x -=
            0.5f * m_width * (V0_S(i + 1, j, k).x - V0_S(i - 1, j, k).x);
        V(i, j, k).y -=
            0.5f * m_height * (V0_S(i + 1, j, k).x - V0_S(i - 1, j, k).x);
        V(i, j, k).z -=
            0.5f * m_depth * (V0_S(i + 1, j, k).x - V0_S(i - 1, j, k).x);
      }
    }
  }

  v0->setBorder(VectorField::BorderKind::CONTAINED);
}

// -------------------------------------------------------------------------- //

void WindSimulation::debugDraw(FieldKind kind, const bs::Vector3 &offset,
                               bool drawFrame) {
  bs::DebugDraw::instance().clear();

  // Draw correct type
  switch (kind) {
  case FieldKind::DENSITY: {
    getDensityField()->debugDraw(offset, drawFrame);
    break;
  }
  case FieldKind::VELOCITY: {
    getVelocityField()->debugDraw(offset, drawFrame);
    break;
  }
  case FieldKind::OBSTRUCTION: {
    getObstructionField()->debugDraw(offset, drawFrame);
    break;
  }
  default:
    break;
  }
}

// -------------------------------------------------------------------------- //

WindSimulation *
WindSimulation::createFromScene(const bs::SPtr<bs::SceneInstance> &scene,
                                const bs::Vector3 &extent,
                                const bs::Vector3 &position, f32 cellSize) {
  // Pre-conditions
  assert(extent.x != 0 && extent.y != 0 && extent.z != 0 &&
         "Extent of wind simulation must not be zero in any dimension");

  // Create simulation
  WindSimulation *sim = new WindSimulation;
  sim->m_cellSize = cellSize;

  // Dimensions
  f32 mul = 1 / cellSize;
  sim->m_width = u32(extent.x * mul);
  sim->m_height = u32(extent.y * mul);
  sim->m_depth = u32(extent.z * mul);

  // Allocate buffers
  sim->m_obstructionField =
      ObstructionField::buildForScene(scene, extent, position, cellSize);
  for (u32 i = 0; i < BUFFER_COUNT; i++) {
    sim->m_densityFields[i] = new DensityField(sim->m_width, sim->m_height,
                                               sim->m_depth, sim->m_cellSize);
    sim->m_velocityFields[i] = new VectorField(sim->m_width, sim->m_height,
                                               sim->m_depth, sim->m_cellSize);
  }

  // Assert post-conditions
  assert(sim->getDensityField()->getDim() ==
             sim->getVelocityField()->getDim() &&
         "Density and vector field must have the same dimensions");
  assert(sim->getDensityField()->getDim() ==
             sim->getObstructionField()->getDim() &&
         "Density and obstruction field must have the same dimensions");

  // Initialize and return
  sim->init();
  return sim;
}
} // namespace wind

// ========================================================================== //
// Macro Undef
// ========================================================================== //

#undef F_DEF
#undef P
#undef P0
#undef V
#undef V0
#undef P_S
#undef P0_S
#undef V_S
#undef V0_S