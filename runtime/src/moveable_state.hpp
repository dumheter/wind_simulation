#ifndef MOVEABLE_STATE_HPP_
#define MOVEABLE_STATE_HPP_

#include "Math/BsQuaternion.h"
#include "Math/BsVector3.h"
#include "Scene/BsSceneObject.h"
#include "Scene/BsTransform.h"
#include "common.hpp"
#include "creator.hpp"
#include "utility/unique_id.hpp"
#include <alflib/memory/raw_memory_reader.hpp>
#include <alflib/memory/raw_memory_writer.hpp>

namespace wind {

class MoveableState {

public:
  MoveableState()
      : m_id(UniqueId::kInvalid), m_type(Creator::Types::kInvalid),
        m_position(bs::Vector3::ONE), m_scale(bs::Vector3::ONE),
        m_rotation(bs::Quaternion::IDENTITY) {}

  MoveableState(UniqueId id) : MoveableState() { m_id = id; }

  static MoveableState generateNew() {
    return MoveableState{UniqueIdGenerator::next()};
  }

  UniqueId getUniqueId() const { return m_id; }

  void setUniqueId(UniqueId id) { m_id = id; }

  Creator::Types getType() const { return m_type; }

  void setType(Creator::Types type) { m_type = type; }

  bs::Vector3 getPosition() const { return m_position; }

  void setPosition(bs::Vector3 position) { m_position = position; }

  bs::Vector3 getScale() const { return m_scale; }

  void setScale(bs::Vector3 scale) { m_scale = scale; }

  bs::Quaternion getRotation() const { return m_rotation; }

  void setRotation(bs::Quaternion rotation) { m_rotation = rotation; }

  void from(const bs::Transform &transform);

  void from(bs::HSceneObject &so);

  /**
   * Does not overwrite the unique id!
   */
  void from(const MoveableState &other);

  void to(bs::Transform &transform) const;

  void to(bs::HSceneObject &so) const;

  bool ToBytes(alflib::RawMemoryWriter &mw) const;

  static MoveableState FromBytes(alflib::RawMemoryReader &mr);

private:
  UniqueId m_id;
  Creator::Types m_type;
  bs::Vector3 m_position;
  bs::Vector3 m_scale;
  bs::Quaternion m_rotation;
};
} // namespace wind

#endif // MOVEABLE_STATE_HPP_
