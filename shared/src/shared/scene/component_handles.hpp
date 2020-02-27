#pragma once

#include "Scene/BsGameObjectHandle.h"

namespace wind {

class CNetComponent;
class CRotor;
class CMyPlayer;
class CWindSource;
class FPSWalker;
class FPSCamera;

using HCNetComponent = bs::GameObjectHandle<CNetComponent>;
using HCRotor = bs::GameObjectHandle<CRotor>;
using HCMyPlayer = bs::GameObjectHandle<CMyPlayer>;
using HCWindSource = bs::GameObjectHandle<CWindSource>;
using HFPSWalker = bs::GameObjectHandle<FPSWalker>;
using HFPSCamera = bs::GameObjectHandle<FPSCamera>;

} // namespace wind
