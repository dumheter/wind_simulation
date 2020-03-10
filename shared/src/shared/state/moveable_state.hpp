#pragma once

#include <BsApplication.h>
#include <Components/BsCRigidbody.h>
#include <Math/BsQuaternion.h>
#include <Math/BsVector3.h>
#include <Scene/BsSceneObject.h>
#include <Scene/BsTransform.h>
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

#include "shared/types.hpp"
#include "shared/utility/unique_id.hpp"
#include "shared/utility/util.hpp"

namespace wind {

struct MoveableState {

  explicit MoveableState(UniqueId id);

  MoveableState() : MoveableState(UniqueId::invalid()) {}

  static MoveableState generateNew() {
    return MoveableState{UniqueIdGenerator::next()};
  }

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

  static MoveableState FromBytes(alflib::RawMemoryReader &mr);

private:
  using Bitfield = u8;
  static constexpr Bitfield kFlagRigid = 0;

public:
  bool getRigid() const { return bitCheck(flag, kFlagRigid); }

  void setRigid(bool isRigid) { bitSet(flag, kFlagRigid, (Bitfield)isRigid); }

 public:
  UniqueId id;
  Bitfield flag;
  bs::Vector3 position;
  bs::Vector3 vel;
  bs::Vector3 angVel;
  bs::Quaternion rotation;
};

} // namespace wind
