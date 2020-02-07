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
// Editor Declaration
// ========================================================================== //

namespace wind {

WindSimulation::~WindSimulation() {
  //
}

// -------------------------------------------------------------------------- //

void WindSimulation::step(f32 delta) {
  // Sources
  // m_bufferIndex = (m_bufferIndex + 1) % BUFFER_COUNT;

  // Diffusion
  stepDensityDiffusion(delta);
  m_bufferIndex = (m_bufferIndex + 1) % BUFFER_COUNT;

  // Motion
  // stepDensityAdvection(delta);
}

// -------------------------------------------------------------------------- //

void WindSimulation::init() {
  // Initialize density fields
  for (u32 i = 0; i < getDensityField()->getDataSize(); i++) {
    DensityField::Pos pos = getDensityField()->fromOffset(i);
    for (u32 j = 0; j < BUFFER_COUNT; j++) {
      m_densityFields[j]->get(i) =
          getDensityField()->isEdgePos(pos)
              ? 0
              : i / f32(getDensityField()->getDataSize() - 1);
    }
  }

  // Initialize the vector fields
  for (u32 i = 0; i < getVectorField()->getDataSize(); i++) {
    VectorField::Pos pos = getVectorField()->fromOffset(i);
    for (u32 j = 0; j < BUFFER_COUNT; j++) {
      m_vectorFields[j]->get(i) = getVectorField()->isEdgePos(pos)
                                      ? bs::Vector3()
                                      : bs::Vector3(1, 0, 0);
    }
  }

  // Set density border conditions
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepDensityDiffusion(f32 delta) {

  f32 diffusion = 0.1f;
  f32 a = delta * diffusion * m_width * m_height * m_depth;

  // Fields
  DensityField *p = getDensityField();
  DensityField *p0 = getDensityFieldPrev();

  // Run 20 iterations
  for (u32 k = 0; k < 20; k++) {
    // For each cell
    for (u32 z = 1; z < m_depth - 1; z++) {
      for (u32 y = 1; y < m_height - 1; y++) {
        for (u32 x = 1; x < m_width - 1; x++) {
          f32 b = p->get(x - 1, y, z) + p->get(x + 1, y, z) +
                  p->get(x, y - 1, z) + p->get(x, y + 1, z) +
                  p->get(x, y, z - 1) + p->get(x, y, z + 1);
          p->get(x, y, z) = (p0->get(x, y, z) + a * b) / (1.0f + 6.0f * a);
        }
      }
    }
  }

  // Set border conditions
  setDensityBorderCond();
}

// -------------------------------------------------------------------------- //

void WindSimulation::stepDensityAdvection(f32 delta) {
  //
  f32 dt0 = delta;

  // Fields
  DensityField *p = getDensityField();
  DensityField *p0 = getDensityFieldPrev();
  VectorField *v = getVectorField();
  VectorField *v0 = getVectorFieldPrev();

  for (u32 z = 1; z < m_depth - 1; z++) {
    for (u32 y = 1; y < m_height - 1; y++) {
      for (u32 x = 1; x < m_width - 1; x++) {
        const bs::Vector3 &vec = v->get(x, y, z);
        f32 _x = x - dt0 * vec.x;
        f32 _y = y - dt0 * vec.y;
        f32 _z = z - dt0 * vec.z;

        // _x
        if (_x < 0.5f) {
          _x = 0.5f;
        } else if (_x > m_width + 0.5f) {
          _x = m_width + 0.5f;
        }
        u32 x0 = u32(_x);
        u32 x1 = x0 + 1;

        // _y
        if (_y < 0.5f) {
          _y = 0.5f;
        } else if (_y > m_height + 0.5f) {
          _y = m_height + 0.5f;
        }
        u32 y0 = u32(_y);
        u32 y1 = y0 + 1;

        // _z
        if (_z < 0.5f) {
          _z = 0.5f;
        } else if (_z > m_depth + 0.5f) {
          _z = m_depth + 0.5f;
        }
        u32 z0 = u32(_z);
        u32 z1 = z0 + 1;

        //
        f32 s1 = _x - x0;
        f32 s0 = 1 - s1;
        f32 t1 = _y - y0;
        f32 t0 = 1 - t1;
        f32 u1 = _z - z0;
        f32 u0 = 1 - u1;

        //
        p->get(x, y, z);
      }
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::setDensityBorderCond() {
  // Fields
  DensityField *p = getDensityField();
  DensityField *p0 = getDensityFieldPrev();

  // Set X bounds
  for (u32 z = 1; z < m_depth - 1; z++) {
    for (u32 y = 1; y < m_height - 1; y++) {
      p->get(0, y, z) = p->get(1, y, z);
      p->get(m_width - 1, y, z) = p->get(m_width - 2, y, z);
    }
  }
  // Set Y bounds
  for (u32 z = 1; z < m_depth - 1; z++) {
    for (u32 x = 1; x < m_width - 1; x++) {
      p->get(x, 0, z) = p->get(x, 1, z);
      p->get(x, m_height - 1, z) = p->get(x, m_height - 2, z);
    }
  }
  // Set Z bounds
  for (u32 y = 1; y < m_height - 1; y++) {
    for (u32 x = 1; x < m_width - 1; x++) {
      p->get(x, y, 0) = p->get(x, y, 1);
      p->get(x, y, m_depth - 1) = p->get(x, y, m_depth - 2);
    }
  }
}

// -------------------------------------------------------------------------- //

void WindSimulation::debugDraw(FieldKind kind, const bs::Vector3 &offset,
                               bool drawFrame) {
  bs::DebugDraw::instance().clear();

  bs::Vector3 off = offset - (bs::Vector3::ONE * m_cellSize);

  // Draw correct type
  switch (kind) {
  case FieldKind::DENSITY: {
    getDensityField()->debugDraw(off, drawFrame);
    break;
  }
  case FieldKind::VECTOR: {
    getVectorField()->debugDraw(off, drawFrame);
    break;
  }
  case FieldKind::OBSTRUCTION: {
    getObstructionField()->debugDraw(off, drawFrame);
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
  WindSimulation *sim = new WindSimulation;
  sim->m_cellSize = cellSize;

  // Dimensions
  f32 mul = 1 / cellSize;
  sim->m_width = u32(extent.x * mul) + 2;
  sim->m_height = u32(extent.y * mul) + 2;
  sim->m_depth = u32(extent.z * mul) + 2;

  // Allocate buffers
  sim->m_obstructionField = ObstructionField::buildForScene(
      scene, extent + (bs::Vector3::ONE),
      position - (bs::Vector3::ONE * cellSize), cellSize);
  for (u32 i = 0; i < BUFFER_COUNT; i++) {
    sim->m_densityFields[i] = new DensityField(sim->m_width, sim->m_height,
                                               sim->m_depth, sim->m_cellSize);
    sim->m_vectorFields[i] = new VectorField(sim->m_width, sim->m_height,
                                             sim->m_depth, sim->m_cellSize);
  }

  // Assert post-conditions
  assert(sim->getDensityField()->getDim() == sim->getVectorField()->getDim() &&
         "Density and vector field must have the same dimensions");
  assert(sim->getDensityField()->getDim() ==
             sim->getObstructionField()->getDim() &&
         "Density and obstruction field must have the same dimensions");

  // Initialize and return
  sim->init();
  return sim;
}
} // namespace wind