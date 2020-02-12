#ifndef CMYPLAYER_HPP_
#define CMYPLAYER_HPP_

#include "BsPrerequisites.h"
#include "Scene/BsComponent.h"
#include "network/client.hpp"

namespace wind {

/**
 * Component who is owner of the client. Makes sure it is run
 * every update.
 */
class CMyPlayer : public bs::Component {
public:
  CMyPlayer(bs::HSceneObject parent, World *world);

  void setUniqueId(UniqueId uid) { m_client.setUid(uid); }

  UniqueId getUniqueId() const { return m_client.getUid(); }

  bool isConnected() const;

  void connect(const char *address);

  void disconnect();

  void update() override;

  Client &getClient() { return m_client; }

private:
  Client m_client;
};

using HCMyPlayer = bs::GameObjectHandle<CMyPlayer>;
} // namespace wind

#endif // CMYPLAYER_HPP_
