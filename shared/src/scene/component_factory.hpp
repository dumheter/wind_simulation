#pragma once

#include "BsPrerequisites.h"
#include "scene/component_handles.hpp"
#include "types.hpp"

namespace wind {

enum class ComponentTypes : u32 { kInvalid = 0, kPlayer, kCube, kBall, kRotor };

class MoveableState;
class Creator;

// ============================================================ //

class ComponentFactory {
public:
  explicit ComponentFactory();

  /**
   * Based on type info in moveableState, call the correct
   * create function.
   */
  HCNetComponent create(const MoveableState &moveableState) const;

  HCNetComponent cube(const MoveableState &moveableState) const;

  HCNetComponent ball(const MoveableState &moveableState) const;

  HCNetComponent rotor(const MoveableState &moveableState) const;

  void floor() const;

private:
  bs::HPhysicsMaterial physicsMaterial();
  bs::HMaterial material(const bs::String &path);

private:
  bs::HMaterial m_matGrid;
  bs::HMaterial m_matGrid2;
  bs::HPhysicsMaterial m_physicsMatStd;
  bs::HMesh m_meshBall;
  bs::HMesh m_meshCube;
  bs::HMesh m_meshRotor;

  friend class Creator;
};
} // namespace wind
