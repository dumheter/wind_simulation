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
  //
}

// -------------------------------------------------------------------------- //

void WindSimulation::step(f32 delta) {
  F_DEF;

  // Density sources and sinks
  if (m_densitySourceActive || m_densitySinkActive) {
    if (m_densitySourceActive) {
      P(0, 0, 0) = 10.0f;
      P(0, 1, 0) = 10.0f;
      P(0, 2, 0) = 10.0f;
      P(0, 3, 0) = 10.0f;
      P(0, 4, 0) = 10.0f;
      m_densitySourceActive = false;
    }
    if (m_densitySinkActive) {
      P(m_width - 1, 2, m_depth - 1) = 0.0f;
      P(m_width - 1, 3, m_depth - 1) = 0.0f;
      P(m_width - 1, 4, m_depth - 1) = 0.0f;
      P(m_width - 1, 5, m_depth - 1) = 0.0f;
    }
    updateDensityBufferIndex();
  }

  // Density Diffusion
  if (m_densityDiffusionActive) {
    stepDensityDiffusion(delta);
    updateDensityBufferIndex();
  }

  // Density Advection
  if (m_densityAdvectionActive) {
    stepDensityAdvection(delta);
    updateDensityBufferIndex();
  }

  // Velocity Sources

  // Velocity Diffusion
  if (m_velocityDiffusionActive) {
    stepVelocityDiffusion(delta);
    updateVelocityBufferIndex();
  }

  // Velocity Advection
  if (m_velocityAdvectionActive) {
    stepVelocityAdvection(delta);
    updateVelocityBufferIndex();
  }
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
      m_velocityFields[j]->get(i) = bs::Vector3(1, 0, 1);
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepDensityDiffusion(f32 delta) {
  F_DEF

  f32 dtDivDx = delta / m_cellSize;
  f32 dtDivDy = delta / m_cellSize;
  f32 dtDivDz = delta / m_cellSize;
  f32 dtDivDxDx = dtDivDx / m_cellSize;
  f32 dtDivDyDy = dtDivDy / m_cellSize;
  f32 dtDivDzDz = dtDivDz / m_cellSize;
  f32 denLambdaX = m_viscosity * dtDivDxDx;
  f32 denLambdaY = m_viscosity * dtDivDyDy;
  f32 denLambdaZ = m_viscosity * dtDivDzDz;
  f32 denGamma0 = 1 / (1 + 2 * (denLambdaX + denLambdaY + denLambdaZ));
  f32 denGammaX = denLambdaX * denGamma0;
  f32 denGammaY = denLambdaY * denGamma0;
  f32 denGammaZ = denLambdaZ * denGamma0;

  f32 a = delta * m_diffusion * m_width * m_height * m_depth;

  /*
  f32 dim = f32(
      wind::max(wind::max(m_width, m_height), wind::max(m_height, m_depth)));
  f32 a = delta * m_diffusion * dim * dim * dim;
  */

  // Gauss seidel iterations
  for (u32 k = 0; k < GAUSS_SEIDEL_STEPS; k++) {
    // For each cell
    for (u32 k = 0; k < m_depth; k++) {
      for (u32 j = 0; j < m_height; j++) {
        for (u32 i = 0; i < m_width; i++) {

#if 1
          f32 b = P_S(i - 1, j, k) + P_S(i + 1, j, k) + P_S(i, j - 1, k) +
                  P_S(i, j + 1, k) + P_S(i, j, k - 1) + P_S(i, j, k + 1);
          P(i, j, k) = (P0_S(i, j, k) + a * b) / (1.0f + 6.0f * a);
#else

          P(i, j, k) = denGamma0 * P0(i, j, k) +
                       denGammaX * (P_S(i + 1, j, k) + P_S(i - 1, j, k)) +
                       denGammaY * (P_S(i, j + 1, k) + P_S(i, j - 1, k)) +
                       denGammaZ * (P_S(i, j, k + 1) + P_S(i, j, k - 1));
#endif
        }
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepDensityAdvection(f32 delta) {
  F_DEF

  f32 dim = f32(
      wind::max(wind::max(m_width, m_height), wind::max(m_height, m_depth)));
  f32 dtx = delta * dim;
  f32 dty = delta * dim;
  f32 dtz = delta * dim;

  for (s32 k = 0; k < s32(m_depth); k++) {
    for (s32 j = 0; j < s32(m_height); j++) {
      for (s32 i = 0; i < s32(m_width); i++) {
        bs::Vector3 &vel = V0(i, j, k);

        f32 iPrev = clamp(i - dtx * vel.x, 0.5f, m_width - 0.5f);
        s32 i0 = s32(std::floor(iPrev));
        s32 i1 = i0 + 1;
        f32 a1 = iPrev - i0, a0 = 1 - a1;

        f32 jPrev = clamp(j - dty * vel.y, 0.5f, m_height - 0.5f);
        s32 j0 = s32(std::floor(jPrev));
        s32 j1 = j0 + 1;
        f32 b1 = jPrev - j0, b0 = 1 - b1;

        f32 kPrev = clamp(k - dtz * vel.z, 0.5f, m_depth - 0.5f);
        s32 k0 = s32(std::floor(kPrev));
        s32 k1 = k0 + 1;
        f32 c1 = kPrev - k0, c0 = 1 - c1;

#if 1
        P(i, j, k) =
            c0 * (b0 * (a0 * P0_S(i0, j0, k0) + a1 * P0_S(i1, j0, k0)) +
                  b1 * (a0 * P0_S(i0, i1, k0) + a1 * P0_S(i1, j1, k0))) +
            c1 * (b0 * (a0 * P0_S(i0, j0, k1) + a1 * P0_S(i1, j0, k1)) +
                  b1 * (a0 * P0_S(i0, i1, k1) + a1 * P0_S(i1, j1, k1)));
#else

        P(i, j, k) =
            a0 * (b0 * c0 * P0(i0, j0, k0) + b1 * c0 * P0(i0, j1, k0) +
                  b0 * c1 * P0(i0, j0, k1) + b1 * c1 * P0(i0, j1, k1)) +
            a1 * (b0 * c0 * P0(i1, j0, k0) + b1 * c0 * P0(i1, j1, k0) +
                  b0 * c1 * P0(i1, j0, k1) + b1 * c1 * P0(i1, j1, k1));
#endif
      }
    }
  }
}

// -------------------------------------------------------------------------- //

/*
void WindSimulation::setDensityBorderCond() {
  F_DEF

  // Set X bounds
  for (u32 z = 0; z < m_depth; z++) {
    for (u32 y = 0; y < m_height; y++) {
      P(0, y, z) = P(1, y, z);
      P(m_width - 1, y, z) = P(m_width - 2, y, z);
    }
  }
  // Set Y bounds
  for (u32 z = 01; z < m_depth; z++) {
    for (u32 x = 0; x < m_width; x++) {
      P(x, 0, z) = P(x, 1, z);
      P(x, m_height - 1, z) = P(x, m_height - 2, z);
    }
  }
  // Set Z bounds
  for (u32 y = 0; y < m_height; y++) {
    for (u32 x = 0; x < m_width; x++) {
      P(x, y, 0) = P(x, y, 1);
      P(x, y, m_depth - 1) = P(x, y, m_depth - 2);
    }
  }
}
*/

// --------------------------------------------------------------------------
// //

void WindSimulation::stepVelocityDiffusion(f32 delta) {
  F_DEF

  for (u32 k = 0; k < m_depth; k++) {
    for (u32 j = 0; j < m_height; j++) {
      for (u32 i = 0; i < m_width; i++) {
        V(i, j, k) = V0(i, j, k);
      }
    }
  }

  // setVelocityBorderCond();
}

// --------------------------------------------------------------------------
// //

void WindSimulation::stepVelocityAdvection(f32 delta) {
  F_DEF

  for (u32 k = 0; k < m_depth; k++) {
    for (u32 j = 0; j < m_height; j++) {
      for (u32 i = 0; i < m_width; i++) {
        V(i, j, k) = V0(i, j, k);
      }
    }
  }

  // setVelocityBorderCond();
}

// --------------------------------------------------------------------------
// //

/*
void WindSimulation::setVelocityBorderCond() {
  F_DEF

  // Set X bounds
  for (u32 z = 0; z < m_depth; z++) {
    for (u32 y = 0; y < m_height; y++) {
      p->get(0, y, z) = p->getSafe(-1, y, z);
      p->get(m_width - 1, y, z) = p->getSafe(m_width, y, z);
    }
  }
  // Set Y bounds
  for (u32 z = 01; z < m_depth; z++) {
    for (u32 x = 0; x < m_width; x++) {
      p->get(x, 0, z) = p->getSafe(x, -1, z);
      p->get(x, m_height - 1, z) = p->getSafe(x, m_height, z);
    }
  }
  // Set Z bounds
  for (u32 y = 0; y < m_height; y++) {
    for (u32 x = 0; x < m_width; x++) {
      p->get(x, y, 0) = p->getSafe(x, y, -1);
      p->get(x, y, m_depth - 1) = p->getSafe(x, y, m_depth);
    }
  }
}
*/

// --------------------------------------------------------------------------
// //

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

// --------------------------------------------------------------------------
// //

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