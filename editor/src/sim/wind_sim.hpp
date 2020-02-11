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
 * The second buffer is the velocity field. This contains the wind direction and
 * strength at the different positions.
 *
 * The third, and last, buffer is the obstruction field. This field has binary
 * cells, used to determine whether or not the cell is obstructed by objects in
 * the scene. This buffer is also not multi-buffered as it's static.
 *
 *
 *
 * To update the simulation of the wind, the 'step' function must be called.
 * This function takes in the delta time (time between frames) that should be
 * used to update the simulation for the correct timespan. Supplying the same
 * delta time as that of the game means that the simulation will run in
 * real-time.
 *
 * -----------------------------------------------------------------------------
 *
 * Algorithms are adapted from the 2003 paper by Jos Stam "Real-Time Fluid
 * Dynamics for Games". The 3D extension by Blain Maguire
 * (Link: https://github.com/BlainMaguire/3dfluid) has also been used.
 *
 * The implementation of field (Field) used by the wind simulation supports
 * sampling cells outside the bounds using the 'getSafe' function. This means
 * that unlike the two mentioned texts we do not need to set boundary conditions
 * after every action, the values can instead simply be sampled outside the
 * field. Depending on where outside the lie (face, edge, corner) the sampled
 * value matches that set by the 'set_bnd' function in those texts.
 *
 * For the velocity fields (VectorField) the values sampled outside the
 * boundaries can be changed by calling 'setBorderKind'
 * (See wind::VectorField::BorderKind) for more information about the different
 * border kinds.
 *
 */
class WindSimulation {
public:
  /* Type of debug data to show */
  enum class FieldKind { DENSITY, VELOCITY, OBSTRUCTION };

public:
  /* Destruct wind simulation along with data */
  ~WindSimulation();

  /* Step the simulation with the specified delta time (dt). Stepping the
   * delta-time with the real frame-time means that the simulation should run in
   * real-time */
  void step(f32 delta);

  /* Step density simulation */
  void stepDensity(f32 delta);

  /* Step velocity simulation */
  void stepVelocity(f32 delta);

  /* Add density sources */
  void addDensitySource() { m_addDensitySource = true; }

  /* Add density sinks */
  void addDensitySink() { m_addDensitySink = true; }

  /* Enable or disable diffusion simulation for the density field */
  void setDensityDiffusionActive(bool active) {
    m_densityDiffusionActive = active;
  }

  /* Enable or disable advection simulation for the density field */
  void setDensityAdvectionActive(bool active) {
    m_densityAdvectionActive = active;
  }

  /* Enable or disable diffusion simulation for the velocity field */
  void setVelocityDiffusionActive(bool active) {
    m_velocityDiffusionActive = active;
  }

  /* Enable or disable advection simulation for the velocity field */
  void setVelocityAdvectionActive(bool active) {
    m_velocityAdvectionActive = active;
  }

  /* Draw debug information */
  void debugDraw(FieldKind kind,
                 const bs::Vector3 &offset = bs::Vector3(0, 0, 0),
                 bool drawFrame = true);

  /* Returns the current density field */
  DensityField *getDensityField() {
    return m_densityFields[m_densityBufferIdx];
  }

  /* Returns the current density field */
  DensityField *getDensityFieldPrev() {
    return m_densityFields[(m_densityBufferIdx - 1) % BUFFER_COUNT];
  }

  /* Returns the current vector field */
  VectorField *getVelocityField() {
    return m_velocityFields[m_densityBufferIdx];
  }

  /* Returns the current vector field */
  VectorField *getVelocityFieldPrev() {
    return m_velocityFields[(m_densityBufferIdx - 1) % BUFFER_COUNT];
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

  /* Update density buffer index  */
  void updateDensityBufferIndex(s32 value = 1) {
    m_densityBufferIdx = (m_densityBufferIdx + value) % BUFFER_COUNT;
  }

  /* Run the velocity diffusion */
  void stepVelocityDiffusion(f32 delta);

  /* Run the velocity advection */
  void stepVelocityAdvection(f32 delta);

  /* Update density buffer index  */
  void updateVelocityBufferIndex(s32 value = 1) {
    m_velocityBufferIdx = (m_velocityBufferIdx + value) % BUFFER_COUNT;
  }

public:
  /* Construct a wind simulation from a scene. All data required is determined
   * from the objects in the scene */
  static WindSimulation *createFromScene(
      const bs::SPtr<bs::SceneInstance> &scene, const bs::Vector3 &extent,
      const bs::Vector3 &position = bs::Vector3(), f32 cellSize = 1.0);

private:
  /* Number of buffers flipped between in simulation */
  static constexpr u32 BUFFER_COUNT = 2;

  /* Number of iterations in the gauss-seidel method */
  static constexpr u32 GAUSS_SEIDEL_STEPS = 10;

private:
  /* Dimensions */
  u32 m_width = 0, m_height = 0, m_depth = 0;
  /* Cell size in meters */
  f32 m_cellSize = 1.0f;

  /* Current density buffer index */
  u32 m_densityBufferIdx = 0;
  /* Current velocity buffer index */
  u32 m_velocityBufferIdx = 0;

  /* Diffusion rate */
  f32 m_diffusion = 0.00001f;
  /* Viscosity */
  f32 m_viscosity = 0.0f;

  /* Density fields */
  DensityField *m_densityFields[BUFFER_COUNT];
  /* Velocity fields */
  VectorField *m_velocityFields[BUFFER_COUNT];
  /* Obstruction field */
  ObstructionField *m_obstructionField;

  /* Whether to add density sources */
  bool m_addDensitySource = false;
  /* Whether to add density sinks */
  bool m_addDensitySink = false;

  /* Whether density diffusion is enabled */
  bool m_densityDiffusionActive = true;
  /* Whether density advection is enabled */
  bool m_densityAdvectionActive = true;

  /* Whether velocity diffusion is enabled */
  bool m_velocityDiffusionActive = true;
  /* Whether velocity advection is enabled */
  bool m_velocityAdvectionActive = true;
};

} // namespace wind