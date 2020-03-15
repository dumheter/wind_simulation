#pragma once

#include <RTTI/BsMathRTTI.h>
#include <Reflection/BsRTTIPlain.h>
#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <Scene/BsSceneObject.h>

#include "shared/scene/rtti.hpp"

namespace wind
{

/// A component that will make the object be affected by wind.
/// It querys the global wind system, and applies any wind force.
class CWindAffectable : public bs::Component {
  friend class CWindAffectableRTTI;

public:
  /// Default constructor required for serialization
  CWindAffectable() = default;

  CWindAffectable(const bs::HSceneObject &parent);

  void fixedUpdate() override;

  static bs::RTTITypeBase *getRTTIStatic();

  bs::RTTITypeBase *getRTTI() const override;

private:
};

class CWindAffectableRTTI
    : public bs::RTTIType<CWindAffectable, bs::Component, CWindAffectableRTTI> {
private:
  BS_BEGIN_RTTI_MEMBERS
  BS_END_RTTI_MEMBERS

public:
  const bs::String &getRTTIName() override {
    static bs::String name = "CWindAffectable";
    return name;
  }

  bs::UINT32 getRTTIId() override { return TID_CWindAffectable; }

  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CWindAffectable>();
  }
};
}
