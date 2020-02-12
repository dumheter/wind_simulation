#ifndef CREATOR_HPP_
#define CREATOR_HPP_

#include "BsPrerequisites.h"
#include "common.hpp"

namespace wind {

class CNetComponent;
using HCNetComponent = bs::GameObjectHandle<CNetComponent>;

class World;
class MoveableState;

class Creator {
public:
  enum class Types : u32 {
    kInvalid = 0,
    kPlayer,
    kCube,
    kBall,
  };

  Creator(World *world);

  /**
   * Based on type info in moveableState, call the correct
   * creator function.
   */
  HCNetComponent create(const MoveableState &moveableState) const;

  HCNetComponent player(const MoveableState &moveableState) const;

  HCNetComponent cube(const MoveableState &moveableState) const;

  HCNetComponent ball(const MoveableState &moveableState) const;

  void floor() const;

private:
  bs::HPhysicsMaterial physicsMaterial();
  bs::HMaterial material(const bs::String &path);

private:
  World *m_world;
  bs::HMaterial m_matGrid;
  bs::HMaterial m_matGrid2;
  bs::HPhysicsMaterial m_physicsMatStd;
  bs::HMesh m_meshBall;
  bs::HMesh m_meshCube;
};
} // namespace wind

#endif // CREATOR_HPP_
