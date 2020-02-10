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

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include "math/density_field.hpp"
#include "math/obstruction_field.hpp"
#include "math/vector_field.hpp"

// ========================================================================== //
// Editor Declaration
// ========================================================================== //

namespace wind {

/* Class representing the main wind simulation. The wind simulation works with
 * a couple of different buffers. The buffers that are updated are also
 * multi-buffered, meaning that they are flipped through. The one being written
 * to in the current timestep will be read from in a coming timestep.
 *
 * The first buffer is the density buffer. This buffer contains the densities of
 * "fluid" (air) at the different positions.
 *
 * The second buffer is the vector field. This contains the wind direction and
 * strength at the different positions.
 *
 * The third, and last, buffer is the obstruction field. This field has binary
 * cells, used to determine whether or not the cell is obstructed by objects in
 * the scene. This buffer is also not multi-buffered as it's static.
 */
class WindSimulation {
public:
  /* Type of debug data to show */
  enum class FieldKind { DENSITY, VECTOR, OBSTRUCTION };

public:
  /* Destruct wind simulation along with data */
  ~WindSimulation();

  /* Step the simulation with the specified delta time (dt) */
  void step(f32 delta);

  /* Draw debug information */
  void debugDraw(FieldKind kind,
                 const bs::Vector3 &offset = bs::Vector3(0, 0, 0),
                 bool drawFrame = true);

  /* Returns the current density field */
  DensityField *getDensityField() { return m_densityFields[m_bufferIndex]; }

  /* Returns the current density field */
  DensityField *getDensityFieldPrev() {
    return m_densityFields[(m_bufferIndex - 1) % BUFFER_COUNT];
  }

  /* Returns the current vector field */
  VectorField *getVectorField() { return m_vectorFields[m_bufferIndex]; }

  /* Returns the current vector field */
  VectorField *getVectorFieldPrev() {
    return m_vectorFields[(m_bufferIndex - 1) % BUFFER_COUNT];
  }

  /* Returns the obstruction field */
  ObstructionField *getObstructionField() { return m_obstructionField; }

private:
  /* Private constructor */
  WindSimulation() = default;

  /* Initialize the simulation data */
  void init();

  /* Run the density diffusion */
  void stepDensityDiffusion(f32 delta);

  /* Run the density advection */
  void stepDensityAdvection(f32 delta);

  /* Updates the density field according to the border conditions set */
  void setDensityBorderCond();

  void doSomeStuff();

public:
  /* Construct a wind simulation from a scene. All data required is determined
   * from the objects in the scene */
  static WindSimulation *createFromScene(
      const bs::SPtr<bs::SceneInstance> &scene, const bs::Vector3 &extent,
      const bs::Vector3 &position = bs::Vector3(), f32 cellSize = 1.0);

private:
  /* Number of buffers flipped between in simulation */
  static constexpr u32 BUFFER_COUNT = 2;

private:
  /* Dimensions */
  u32 m_width, m_height, m_depth;
  /* Cell size in meters */
  f32 m_cellSize;

  /* Current buffer index */
  u32 m_bufferIndex = 0;

  /* Density fields */
  DensityField *m_densityFields[BUFFER_COUNT];
  /* Vector fields */
  VectorField *m_vectorFields[BUFFER_COUNT];
  /* Obstruction field */
  ObstructionField *m_obstructionField;
};

} // namespace wind