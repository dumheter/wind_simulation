#pragma once
#include <tuple>
#include "app.hpp"
#include "BsApplication.h"
#include "Scene/BsSceneObject.h"
#include "Material/BsMaterial.h"
#include "Importer/BsImporter.h"
#include "BsFPSCamera.h"
#include "bsFPSWalker.h"
#include "Input/BsInput.h"
#include "Components/BsCPlaneCollider.h"
#include "Components/BsCBoxCollider.h"
#include "Components/BsCCharacterController.h"
#include "Components/BsCRenderable.h"
#include "Components/BsCCamera.h"
#include "Components/BsCRigidbody.h"
#include "Physics/BsPhysicsMaterial.h"
#include "Physics/BsPlaneCollider.h"
#include "Physics/BsBoxCollider.h"
#include "Physics/BsRigidbody.h"
#include "Platform/BsCursor.h"
#include "RenderAPI/BsRenderWindow.h"
#include "RenderAPI/BsRenderTarget.h"
#include "Resources/BsBuiltinResources.h"
#include <vector>
#include <stdio.h>
namespace wind {


class World : public App
{
 public:

  World(const App::Info& info);

 private:
  void setupInput();

  bs::HSceneObject createCamera(bs::HSceneObject player);
  bs::HSceneObject createPlayer();
  bs::HSceneObject createCube(bs::HMaterial material,
                              bs::HPhysicsMaterial physicsMaterial);
  bs::HSceneObject createFloor(bs::HMaterial material,
                               bs::HPhysicsMaterial physicsMaterial);
  bs::HPhysicsMaterial createPhysicsMaterial();
  bs::HMaterial createMaterial(const bs::String &path);
  bs::HSceneObject createGUI(bs::HSceneObject camera);


  std::vector<bs::HSceneObject> m_entities;
};


}
