#pragma once

#include <BsPrerequisites.h>
#include <Input/BsVirtualInput.h>
#include <Math/BsQuaternion.h>
#include <Private/RTTI/BsGameObjectRTTI.h>
#include <RTTI/BsMathRTTI.h>
#include <Reflection/BsRTTIPlain.h>
#include <Reflection/BsRTTIType.h>
#include <Scene/BsComponent.h>
#include <memory>

#include "BsFPSWalker.h"
#include "shared/scene/component_factory.hpp"
#include "shared/utility/rtti_types.hpp"
#include "shared/utility/unique_id.hpp"

namespace wind {

class World;
class Client;

class CMyPlayer : public bs::Component {
public:
  CMyPlayer(); // serialization
  CMyPlayer(bs::HSceneObject parent, World *world);

  void setUniqueId(UniqueId uid);

  UniqueId getUniqueId() const;

  bool isConnected() const;

  void connect(const char *address);

  void disconnect();

  void update() override;

  void fixedUpdate() override;

  void onShoot();

  void setShootForce(f32 force) { m_shootForce = force; }

  f32 getShootForce() const { return m_shootForce; }

  void setWeapon(ComponentTypes weapon);

  Client &getClient() { return *m_client; }
  const Client &getClient() const { return *m_client; }

  bs::HFPSWalker getWalker() { return m_fpsWalker; }

  void lookupId(UniqueId uid);

  friend class CMyPlayerRTTI;

  static bs::RTTITypeBase *getRTTIStatic();
  bs::RTTITypeBase *getRTTI() const override;

private:
  World *m_world;
  std::unique_ptr<Client> m_client;
  bs::HFPSWalker m_fpsWalker;
  bs::Quaternion m_lastRotation;
  ComponentTypes m_weapon;
  bs::VirtualButton m_fire1;
  bs::VirtualButton m_fire2;
  f32 m_shootForce = 30.0f;
};

using HCMyPlayer = bs::GameObjectHandle<CMyPlayer>;

class CMyPlayerRTTI
    : public bs::RTTIType<CMyPlayer, bs::Component, CMyPlayerRTTI> {
private:
public:
  const bs::String &getRTTIName() override {
    static bs::String name = "CMyPlayer";
    return name;
  }
  bs::UINT32 getRTTIId() override { return TID_CMyPlayer; }
  bs::SPtr<bs::IReflectable> newRTTIObject() override {
    return bs::SceneObject::createEmptyComponent<CMyPlayer>();
  }
};

} // namespace wind
