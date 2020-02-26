#pragma once

#include <BsPrerequisites.h>

#include "shared/scene/component_handles.hpp"
#include "shared/scene/types.hpp"
#include "shared/types.hpp"

namespace wind {

using ComponentTypes = ObjectTypes;

class MoveableState;
class Creator;

// ============================================================ //

class ComponentFactory {
public:
  ComponentFactory();

  /**
   * Based on type info in moveableState, call the correct
   * create function.
   */
  HCNetComponent create(const MoveableState &moveableState) const;

  HCNetComponent cube(const MoveableState &moveableState) const;

  HCNetComponent ball(const MoveableState &moveableState) const;

  HCNetComponent rotor(const MoveableState &moveableState) const;

  HCNetComponent windSource(const MoveableState &moveableState) const;

  void floor() const;

private:
  bs::HPhysicsMaterial physicsMaterial();
  bs::HMaterial material(const bs::String &path);
  bs::HMaterial transparentMaterial(const bs::String &path);

private:
  bs::HMaterial m_matGrid;
  bs::HMaterial m_matGrid2;
  bs::HMaterial m_matWireframe;
  bs::HPhysicsMaterial m_physicsMatStd;
  bs::HMesh m_meshBall;
  bs::HMesh m_meshCube;
  bs::HMesh m_meshRotor;

  friend class Creator;
};
} // namespace wind