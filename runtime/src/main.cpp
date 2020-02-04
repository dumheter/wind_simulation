#include "app.hpp"
#include "world.hpp"
#include "Scene/BsSceneObject.h"
#include "Material/BsMaterial.h"
#include "Math/BsVector3.h"

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


int main() {
	using namespace wind;
	using namespace bs;

	App app(App::Info{ "Demo", 1280, 720 });

	SetupInput();
	auto gridMaterial = CreateMaterial("res/grid.png");
	auto stdPhysicsMaterial = CreatePhysicsMaterial();
	auto cube = CreateCube(gridMaterial, stdPhysicsMaterial);
	cube->setPosition(Vector3(0.0f, 2.0f, -8.0f));
	auto cube2 = CreateCube(gridMaterial, stdPhysicsMaterial);
	cube2->setPosition(Vector3(0.0f, 4.0f, -8.0f));
	cube2->setRotation(Quaternion(Degree(0), Degree(45), Degree(0)));
	auto floor = CreateFloor(gridMaterial, stdPhysicsMaterial);
	auto player = CreatePlayer();
	auto camera = CreateCamera(player);
	auto gui = CreateGUI(camera);

	app.run();

	return 0;
}