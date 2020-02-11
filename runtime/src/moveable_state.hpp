#ifndef MOVEABLE_STATE_HPP_
#define MOVEABLE_STATE_HPP_

#include "Math/BsQuaternion.h"
#include "Math/BsVector3.h"
#include "Scene/BsSceneObject.h"
#include "Scene/BsTransform.h"
#include "common.hpp"
#include "utility/unique_id.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

namespace wind {

class MoveableState {

public:
  MoveableState(UniqueId id)
      : m_id(id), m_position(), m_scale(), m_rotation(){};

  static MoveableState generateNew() {
    return MoveableState{UniqueIdGenerator::next()};
  }

  UniqueId getUniqueId() const { return m_id; }

  bs::Vector3 getPosition() const { return m_position; }

  bs::Vector3 getScale() const { return m_scale; }

  bs::Quaternion getRotation() const { return m_rotation; }

  void from(const bs::Transform &transform);

  void from(bs::HSceneObject &so);

  void to(bs::Transform &transform) const;

  void to(bs::HSceneObject &so) const;

  bool ToBytes(alflib::RawMemoryWriter &mw) const;

  static MoveableState FromBytes(alflib::RawMemoryReader &mr);

private:
  UniqueId m_id;
  bs::Vector3 m_position;
  bs::Vector3 m_scale;
  bs::Quaternion m_rotation;
};
} // namespace wind

#endif // MOVEABLE_STATE_HPP_
