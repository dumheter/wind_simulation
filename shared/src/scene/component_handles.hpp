#pragma once

#include "Scene/BsGameObjectHandle.h"

namespace wind {

class CNetComponent;
class CRotor;
class CMyPlayer;

using HCNetComponent = bs::GameObjectHandle<CNetComponent>;
using HCRotor = bs::GameObjectHandle<CRotor>;
using HCMyPlayer = bs::GameObjectHandle<CMyPlayer>;

} // namespace wind

namespace bs {

class FPSWalker;
class FPSCamera;

using HFPSWalker = GameObjectHandle<FPSWalker>;
using HFPSCamera = GameObjectHandle<FPSCamera>;

} // namespace bs