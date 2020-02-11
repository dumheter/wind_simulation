#ifndef CMYPLAYER_HPP_
#define CMYPLAYER_HPP_

#include "BsPrerequisites.h"
#include "Scene/BsComponent.h"
#include "network/client.hpp"

namespace wind {

class CMyPlayer : public bs::Component {
public:
  CMyPlayer(bs::HSceneObject parent, World *world);

  bool isConnected() const;

  void connect(const char *address);

  void disconnect();

  void update() override;

private:
  Client m_client;
};

using HCMyPlayer = bs::GameObjectHandle<CMyPlayer>;
} // namespace wind

#endif // CMYPLAYER_HPP_
