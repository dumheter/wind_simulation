#pragma once

#include "Scene/BsGameObjectHandle.h"

namespace wind {

class CNetComponent;
class CRotor;
class CMyPlayer;
class CWindSource;

using HCNetComponent = bs::GameObjectHandle<CNetComponent>;
using HCRotor = bs::GameObjectHandle<CRotor>;
using HCMyPlayer = bs::GameObjectHandle<CMyPlayer>;
using HCWindSource = bs::GameObjectHandle<CWindSource>;

} // namespace wind

namespace bs {

class FPSWalker;
class FPSCamera;

using HFPSWalker = GameObjectHandle<FPSWalker>;
using HFPSCamera = GameObjectHandle<FPSCamera>;

} // namespace bs
