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

#include "shared/types.hpp"

// ========================================================================== //
// Types
// ========================================================================== //

namespace wind {

/// Enumeration of object types. These are used to distinguish between object
/// categories and to specify common information for that object type.
enum class ObjectType : u32 {
  kInvalid = 0, ///< Invalid
  kEmpty,       ///< Empty object with no components
  kPlane,       ///< Flat plane
  kCube,        ///< Cube
  kBall,        ///< Sphere/Ball
  kModel,       ///< Model with a specific mesh
  kPlayer,      ///< Player with corresponding components
  kRotor,       ///< Helicopter rotor
};

// -------------------------------------------------------------------------- //

/// Enumeration of component types.
enum class ComponentType : u32 {
  kRigidbody,  ///< Rigidbody { "restitution", "mass" }
  kWindSource, ///< Wind source { ["basic function"] }
  kRenderable,
  kRotor ///< Rotor { "x-rot", "y-rot", "z-rot" }
};

} // namespace wind
