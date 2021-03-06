﻿// MIT License
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

#include "shared/macros.hpp"
#include "shared/sim/density_field.hpp"
#include "shared/sim/obstruction_field.hpp"
#include "shared/sim/vector_field.hpp"

// ========================================================================== //
// Editor Declaration
// ========================================================================== //

namespace wind {

WIND_FORWARD_DECLARE(Painter);

// -------------------------------------------------------------------------- //

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
  /// Key for debug variable used to determine whether to run simulation.
  static constexpr char kDebugRun[] = "SimDebugRun";
  /// Key for debug variable used to determine the speed to run the sim at.
  static constexpr char kDebugRunSpeed[] = "SimDebugRunSpeed";

  /// Key for debug variable used to determine whether to paint debug info.
  static constexpr char kDebugPaintKey[] = "SimDebugPaint";
  /// Key for debug variable used to determine whether to paint frames
  static constexpr char kDebugPaintFrameKey[] = "SimDebugPaintFrame";
  /// Key for debug variable that is used to determine what field to draw
  static constexpr char kDebugFieldTypeKey[] = "SimDebugFieldType";

  static constexpr char kDebugVelocitySource[] = "SimDebugVS";

  /// Enumeration of the different fields that make up the simulation.
  enum class FieldKind { kDens, kVel, kObstr };

  /// Enumeration that specifies edges of the simulation
  enum class FieldSubKind { kDens, kVelX, kVelY, kVelZ };

public:
  /// Construct wind simulation of given dimensions
  WindSimulation(s32 width, s32 height, s32 depth, f32 cellSize = 1.0f);

  /// Construct wind simulation of given dimensions
  explicit WindSimulation(const Vec3I &dim, f32 cellSize = 1.0f)
      : WindSimulation(dim.x, dim.y, dim.z, cellSize) {}

  /// Destruct wind simulation along with data
  ~WindSimulation() = default;

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

  /// Paint simulation
  void paint(Painter &painter,
             const bs::Vector3 &offset = bs::Vector3(0, 0, 0)) const;

  /// Returns the density field for the current step in the simulation
  DensityField &D() { return m_d; }

  /// Returns the density field for the current step in the simulation
  const DensityField &D() const { return m_d; }

  /// Returns the density field for the previous step in the simulation
  DensityField &D0() { return m_d0; }

  /// Returns the density field for the previous step in the simulation
  const DensityField &D0() const { return m_d0; }

  /// Returns the velocity field for the current step in the simulation
  VectorField &V() { return m_v; }

  /// Returns the velocity field for the current step in the simulation
  const VectorField &V() const { return m_v; }

  /// Returns the velocity field for the previous step in the simulation
  VectorField &V0() { return m_v0; }

  /// Returns the velocity field for the previous step in the simulation
  const VectorField &V0() const { return m_v0; }

  /// Returns the obstruction field
  ObstructionField &O() { return m_o; }

  /// Returns the obstruction field
  const ObstructionField &O() const { return m_o; }

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

  /// Set all vector fields to v.
  void setAsVec(Vec3F v);

  /// Retrieve the dimension of the simulation in cells
  FieldBase::Dim getDim() const {
    return FieldBase::Dim{u32(m_width), u32(m_height), u32(m_depth)};
  }

  /// Retrieve the dimension of the simulation in meters
  Vec3F getDimM() const { return m_v.getDimM(); }

  /// Retrieve the cell size
  f32 getCellSize() const { return m_v.getCellSize(); }

private:
  /// Gauss-Seidel relaxation
  void gaussSeidel(Field<f32> *f, Field<f32> *f0, FieldSubKind edge, f32 a,
                   f32 c);

  /// Run diffusion
  void diffuse(Field<f32> *f, Field<f32> *f0, FieldSubKind edge, f32 coeff,
               f32 delta);

  /// Run advection
  void advect(Field<f32> *f, Field<f32> *f0, VectorField *vecField,
              FieldSubKind edge, f32 delta);

  /// Project velocity
  void project(Field<f32> *u, Field<f32> *v, Field<f32> *w, Field<f32> *prj,
               Field<f32> *div);

  /// Set boundary condition
  void setBoundary(Field<f32> *field, FieldSubKind edge);

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
  DensityField m_d;
  /// Density fields (previous )
  DensityField m_d0;
  /// Velocity field (current)
  VectorField m_v;
  /// Velocity field (previous)
  VectorField m_v0;
  /* Obstruction field */
  ObstructionField m_o;

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
