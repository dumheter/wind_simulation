#ifndef CPLAYER_HPP_
#define CPLAYER_HPP_

#include "BsPrerequisites.h"
#include "Scene/BsComponent.h"
#include "network/client.hpp"

namespace wind {

class CPlayer : public bs::Component {
public:
  CPlayer(const bs::HSceneObject &parent);

  // void update() override;

private:
  bs::HCharacterController m_controller;
};

using HCPlayer = bs::GameObjectHandle<CPlayer>;
} // namespace wind

#endif // CPLAYER_HPP_
