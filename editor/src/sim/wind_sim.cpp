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
  m_bufferIndex = (m_bufferIndex + 1) % BUFFER_COUNT;

  // Diffusion
  stepDensityDiffusion(delta);
  // m_bufferIndex = (m_bufferIndex + 1) % BUFFER_COUNT;

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
          i / f32(getDensityField()->getDataSize() - 1);
    }
  }

  // Initialize the vector fields
  for (u32 i = 0; i < getVectorField()->getDataSize(); i++) {
    VectorField::Pos pos = getVectorField()->fromOffset(i);
    for (u32 j = 0; j < BUFFER_COUNT; j++) {
      m_vectorFields[j]->get(i) = bs::Vector3(1, 0, 1);
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
    for (u32 z = 0; z < m_depth; z++) {
      for (u32 y = 0; y < m_height; y++) {
        for (u32 x = 0; x < m_width; x++) {
          f32 b = p->getSafe(x - 1, y, z) + p->getSafe(x + 1, y, z) +
                  p->getSafe(x, y - 1, z) + p->getSafe(x, y + 1, z) +
                  p->getSafe(x, y, z - 1) + p->getSafe(x, y, z + 1);
          p->get(x, y, z) = (p0->getSafe(x, y, z) + a * b) / (1.0f + 6.0f * a);
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

  for (u32 k = 0; k < m_depth; k++) {
    for (u32 j = 0; j < m_height; j++) {
      for (u32 i = 0; i < m_width; i++) {
        const bs::Vector3 &vec = v->get(i, j, k);
        f32 x = Clamp(i - dt0 * vec.x, 0.5f, m_width + 0.5f);
        f32 y = Clamp(j - dt0 * vec.y, 0.5f, m_height + 0.5f);
        f32 z = Clamp(k - dt0 * vec.z, 0.5f, m_depth + 0.5f);
        u32 i0 = u32(std::floor(x)), i1 = i0 + 1;
        u32 j0 = u32(std::floor(y)), j1 = j0 + 1;
        u32 k0 = u32(std::floor(z)), k1 = k0 + 1;

        f32 a1 = x - i0, a0 = 1 - a1;
        f32 b1 = x - j0, b0 = 1 - b1;
        f32 c1 = x - k0, c0 = 1 - c1;

        f32 ab0 =
            b0 * (a0 * p0->getSafe(i0, j0, k0) + a1 * p0->getSafe(i0, j0, k1));
        f32 ab1 =
            b1 * (a0 * p0->getSafe(i0, j1, k0) + a1 * p0->getSafe(i0, j1, k1));
        f32 ab2 =
            b0 * (a0 * p0->getSafe(i1, j0, k0) + a1 * p0->getSafe(i1, j0, k1));
        f32 ab3 =
            b1 * (a0 * p0->getSafe(i1, j1, k0) + a1 * p0->getSafe(i1, j1, k1));
        f32 v = c0 * (ab0 + ab1) + c1 * (ab2 + ab3);

        p->get(i, j, k) = v;
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
  for (u32 z = 0; z < m_depth; z++) {
    for (u32 y = 0; y < m_height; y++) {
      p->get(0, y, z) = p->getSafe(1, y, z);
      p->get(m_width - 1, y, z) = p->getSafe(m_width - 2, y, z);
    }
  }
  // Set Y bounds
  for (u32 z = 01; z < m_depth; z++) {
    for (u32 x = 0; x < m_width; x++) {
      p->get(x, 0, z) = p->getSafe(x, 1, z);
      p->get(x, m_height - 1, z) = p->getSafe(x, m_height - 2, z);
    }
  }
  // Set Z bounds
  for (u32 y = 0; y < m_height; y++) {
    for (u32 x = 0; x < m_width; x++) {
      p->get(x, y, 0) = p->getSafe(x, y, 1);
      p->get(x, y, m_depth - 1) = p->getSafe(x, y, m_depth - 2);
    }
  }
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
  case FieldKind::VECTOR: {
    getVectorField()->debugDraw(offset, drawFrame);
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