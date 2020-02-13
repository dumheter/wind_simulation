#ifndef CMYPLAYER_HPP_
#define CMYPLAYER_HPP_

#include "BsFPSWalker.h"
#include "BsPrerequisites.h"
#include "Math/BsQuaternion.h"
#include "Scene/BsComponent.h"
#include "network/client.hpp"

namespace wind {

class World;

class CMyPlayer : public bs::Component {
public:
  CMyPlayer(bs::HSceneObject parent, World *world);

  void setUniqueId(UniqueId uid) { m_client.setUid(uid); }

  UniqueId getUniqueId() const { return m_client.getUid(); }

  bool isConnected() const;

  void connect(const char *address);

  void disconnect();

  void update() override;

  void fixedUpdate() override;

  Client &getClient() { return m_client; }

private:
  World *m_world;
  Client m_client;
  bs::HFPSWalker m_fpsWalker;
  bs::Quaternion m_lastRotation;
};

using HCMyPlayer = bs::GameObjectHandle<CMyPlayer>;
} // namespace wind

#endif // CMYPLAYER_HPP_
