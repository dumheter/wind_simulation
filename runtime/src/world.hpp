#pragma once
#include <tuple>
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

namespace wind {
	void SetupInput();

	bs::HSceneObject CreateCamera(bs::HSceneObject player);
	bs::HSceneObject CreatePlayer();
	bs::HSceneObject CreateCube(bs::HMaterial material, bs::HPhysicsMaterial physicsMaterial);
	bs::HSceneObject CreateFloor(bs::HMaterial material, bs::HPhysicsMaterial physicsMaterial);
	bs::HPhysicsMaterial CreatePhysicsMaterial();
	bs::HMaterial CreateMaterial(const bs::String& path);
	bs::HSceneObject CreateGUI(bs::HSceneObject camera);
}