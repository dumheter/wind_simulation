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

int main()
{
    wind::World world{wind::App::Info{"Demo", 1280, 720}};
    world.run();
    return 0;
}
