#ifndef CREATOR_HPP_
#define CREATOR_HPP_

#include "common.hpp"
#include "BsPrerequisites.h"

namespace wind
{

class World;
class MoveableState;

class Creator
{
 public:
  enum class Types : u32 {
    kInvalid = 0,
    kPlayer,
    kCube,
    kBall,
  };

  Creator(World* world);

  /**
   * Based on type info in moveableState, call the correct
   * creator function.
   */
  void create(const MoveableState &moveableState) const;

  void player(const MoveableState &moveableState) const;

  void cube(const MoveableState &moveableState) const;

  void ball(const MoveableState &moveableState) const;

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
}

#endif//CREATOR_HPP_
