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

WindSimulation::WindSimulation(u32 width, u32 height, u32 depth, f32 cellSize)
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

  // Initialize
  for (u32 i = 0; i < m_d->getDataSize(); i++) {
    m_d->get(i) = 0.5f;
    m_v->get(i) = Vec3F(0.8f, 0.1f, 0.6f);
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

void WindSimulation::stepDensity(f32 delta) {}

// -------------------------------------------------------------------------- //

void WindSimulation::stepVelocity(f32 delta) {}

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

} // namespace wind
