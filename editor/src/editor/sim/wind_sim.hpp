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

#include "editor/sim/density_field.hpp"
#include "editor/sim/obstruction_field.hpp"
#include "editor/sim/vector_field.hpp"

// ========================================================================== //
// Editor Declaration
// ========================================================================== //

namespace wind {

/// Class representing the main wind simulation. The wind simulation works with
/// a couple of different buffers. The buffers that are updated are also
/// multi-buffered, meaning that they are flipped through. The one being written
/// to in the current timestep will be read from in a coming timestep.
///
/// The first buffer is the density buffer. This buffer contains the densities
/// of "fluid" (air) at the different positions.
///
/// The second buffer is the velocity field. This contains the wind direction
/// and strength at the different positions.
///
/// The third, and last, buffer is the obstruction field. This field has binary
/// cells, used to determine whether or not the cell is obstructed by objects in
/// the scene. This buffer is also not multi-buffered as it's static.
///
///
///
/// To update the simulation of the wind, the 'step' function must be called.
/// This function takes in the delta time (time between frames) that should be
/// used to update the simulation for the correct timespan. Supplying the same
/// delta time as that of the game means that the simulation will run in
/// real-time.
///
/// -----------------------------------------------------------------------------
///
/// Algorithms are adapted from the 2003 paper by Jos Stam "Real-Time Fluid
/// Dynamics for Games". This solution in this paper only applies to two
/// dimensions but can be extended to three dimensions.
///
/// For the extension to three dimensions the implementation by Blain Maguire
/// (Link: https://github.com/BlainMaguire/3dfluid) has been used as
/// inspiration. The book "Game Physics" by David H. Eberly was also used to
/// verify the solution in three dimensions. In the book the solution is
/// presented in chapter 5.6 "Implementing the simplified 3d model".
///
class WindSimulation {
public:
  /// Enumeration of the different fields that make up the simulation.
  enum class FieldKind { DENSITY, VELOCITY, OBSTRUCTION };

  /// Enumeration that specifies edges of the simulation
  enum class EdgeKind { DENSITY, VELOCITY_X, VELOCITY_Y, VELOCITY_Z };

public:
  /// Private constructor
  WindSimulation(s32 width, s32 height, s32 depth, f32 cellSize = 1.0f);

  /// Destruct wind simulation along with data
  ~WindSimulation();

  /// Build the obstruction field of the simulation for the specified scene. The
  /// position is used to specify at what position the simulation should check
  /// the scenery.
  void buildForScene(const bs::SPtr<bs::SceneInstance> &scene,
                     const bs::Vector3 &position = bs::Vector3());

  /// Step the simulation with the specified delta time (dt). Stepping the
  /// delta-time with the real frame-time means that the simulation should run
  /// in real-time
  void step(f32 delta);

  /// Run the simulation for 'steps' number of steps. For each step the
  /// simulation is run with the specified delta time.
  void stepN(f32 delta, u32 steps);

  /// Step density simulation
  void stepDensity(f32 delta);

  /// Step velocity simulation
  void stepVelocity(f32 delta);

  /// Draw debug information
  void debugDraw(FieldKind kind,
                 const bs::Vector3 &offset = bs::Vector3(0, 0, 0),
                 bool drawFrame = true);

  /// Returns the density field for the current step in the simulation
  DensityField *D() { return m_d; }

  /// Returns the density field for the previous step in the simulation
  DensityField *D0() { return m_d0; }

  /// Returns the velocity field for the current step in the simulation
  VectorField *V() { return m_v; }

  /// Returns the velocity field for the previous step in the simulation
  VectorField *V0() { return m_v0; }

  /// Add density sources
  void addDensitySource() { m_addDensitySource = true; }

  /// Add density sinks
  void addDensitySink() { m_addDensitySink = true; }

  /// Enable or disable diffusion simulation for the density field
  void setDensityDiffusionActive(bool active) {
    m_densityDiffusionActive = active;
  }

  /// Enable or disable advection simulation for the density field
  void setDensityAdvectionActive(bool active) {
    m_densityAdvectionActive = active;
  }

  /// Add density sources
  void addVelocitySource() { m_addVelocitySource = true; }

  /// Add density sinks
  void addVelocitySink() { m_addVelocitySink = true; }

  /// Enable or disable diffusion simulation for the velocity field
  void setVelocityDiffusionActive(bool active) {
    m_velocityDiffusionActive = active;
  }

  /// Enable or disable advection simulation for the velocity field
  void setVelocityAdvectionActive(bool active) {
    m_velocityAdvectionActive = active;
  }

  /// Set the vector field to look like a tornado (rotation around center +
  /// force upwards + force increase with height)
  void setAsTornado();

private:
  /// Gauss-Seidel relaxation
  void gaussSeidel(Field<f32> *f, Field<f32> *f0, EdgeKind edge, f32 a, f32 c);

  /// Run diffusion
  void diffuse(Field<f32> *f, Field<f32> *f0, EdgeKind edge, f32 coeff,
               f32 delta);

  /// Run advection
  void advect(Field<f32> *f, Field<f32> *f0, VectorField *vecField,
              EdgeKind edge, f32 delta);

  /// Project velocity
  void project(Field<f32> *u, Field<f32> *v, Field<f32> *w, Field<f32> *prj,
               Field<f32> *div);

  /// Set boundary condition
  void setBoundary(Field<f32> *field, EdgeKind edge);

private:
  /// Number of iterations in the Gauss-Seidel method
  static constexpr u32 GAUSS_SEIDEL_STEPS = 10u;

private:
  /// Dimensions
  s32 m_width = 0, m_height = 0, m_depth = 0;
  /// Cell size in meters
  f32 m_cellSize = 1.0f;

  /// Current density buffer index
  u32 m_densityBufferIdx = 0;
  /// Current velocity buffer index
  u32 m_velocityBufferIdx = 0;

  /// Diffusion rate
  f32 m_diffusion = 0.001f;
  /// Viscosity
  f32 m_viscosity = 0.0f;

  /// Density fields (current)
  DensityField *m_d = nullptr;
  /// Density fields (previous )
  DensityField *m_d0 = nullptr;
  /// Velocity field (current)
  VectorField *m_v = nullptr;
  /// Velocity field (previous)
  VectorField *m_v0 = nullptr;
  /* Obstruction field */
  ObstructionField *m_o = nullptr;

  /// Whether to add density sources
  bool m_addDensitySource = false;
  /// Whether to add density sinks
  bool m_addDensitySink = false;
  /// Whether density diffusion is enabled
  bool m_densityDiffusionActive = true;
  /// Whether density advection is enabled
  bool m_densityAdvectionActive = true;

  /// Whether to add velocity sources
  bool m_addVelocitySource = false;
  /// Whether to add velocity sinks
  bool m_addVelocitySink = false;
  /// Whether velocity diffusion is enabled
  bool m_velocityDiffusionActive = true;
  /// Whether velocity advection is enabled
  bool m_velocityAdvectionActive = true;
};

} // namespace wind