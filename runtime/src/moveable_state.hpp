#ifndef MOVEABLE_STATE_HPP_
#define MOVEABLE_STATE_HPP_

#include "BsApplication.h"
#include "Components/BsCRigidbody.h"
#include "Math/BsQuaternion.h"
#include "Math/BsVector3.h"
#include "Scene/BsSceneObject.h"
#include "Scene/BsTransform.h"
#include "creator.hpp"
#include "types.hpp"
#include "utility/unique_id.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>
#include "utility/util.hpp"

namespace wind {

class MoveableState {

public:
  MoveableState();

  MoveableState(UniqueId id) : MoveableState() {
    m_id = id;
  }

  static MoveableState generateNew() {
    return MoveableState{UniqueIdGenerator::next()};
  }

  UniqueId getUniqueId() const { return m_id; }

  void setUniqueId(UniqueId id) { m_id = id; }

  Creator::Types getType() const { return m_type; }

  void setType(Creator::Types type) { m_type = type; }

  bs::Vector3 getPosition() const { return m_position; }

  void setPosition(bs::Vector3 position) { m_position = position; }

  bs::Vector3 getVel() const { return m_vel; }

  void setVel(bs::Vector3 vel) { m_vel = vel; }

  bs::Vector3 getAngVel() const { return m_angVel; }

  void setAngVel(bs::Vector3 angVel) { m_angVel = angVel; }

  bs::Quaternion getRotation() const { return m_rotation; }

  void setRotation(bs::Quaternion rotation) { m_rotation = rotation; }

  void from(const bs::Transform &transform);

  void from(bs::HSceneObject so);

  /**
   * Does not overwrite the unique id or type.
   */
  void from(const MoveableState &other);

  void from(bs::HRigidbody rigid);

  void to(bs::Transform &transform) const;

  void to(bs::HSceneObject &so) const;

  bool ToBytes(alflib::RawMemoryWriter &mw) const;

private:
  using Bitfield = u8;
  static constexpr Bitfield kFlagRigid = 0;

public:
  bool getRigid() const { return bitCheck(m_flag, kFlagRigid); }

  void setRigid(bool isRigid) {
    bitSet(m_flag, kFlagRigid, (Bitfield)isRigid);
  }

  static MoveableState FromBytes(alflib::RawMemoryReader &mr);

private:
  UniqueId m_id;
  Creator::Types m_type;
  Bitfield m_flag;
  bs::Vector3 m_position;
  bs::Vector3 m_vel;
  bs::Vector3 m_angVel;
  bs::Quaternion m_rotation;
};
} // namespace wind

#endif // MOVEABLE_STATE_HPP_
