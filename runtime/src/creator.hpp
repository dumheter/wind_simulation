#pragma once

#include "BsPrerequisites.h"
#include "shared/scene/component_factory.hpp"

namespace wind {

class CNetComponent;
using HCNetComponent = bs::GameObjectHandle<CNetComponent>;

class World;
class MoveableState;

class Creator {
public:
  explicit Creator(World *world);

  /**
   * Based on type info in moveableState, call the correct
   * creator function.
   */
  HCNetComponent create(const MoveableState &moveableState) const;

  HCNetComponent player(const MoveableState &moveableState) const;

  HCNetComponent cube(const MoveableState &moveableState) const;

  HCNetComponent ball(const MoveableState &moveableState) const;

  HCNetComponent rotor(const MoveableState &moveableState) const;

  HCNetComponent windSource(const MoveableState &moveableState) const;

  void floor() const;

private:
  World *m_world;
  ComponentFactory m_factory;
};
} // namespace wind
