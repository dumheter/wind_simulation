#include "world.hpp"
#include "GUI/BsGUILayout.h"
#include "GUI/BsGUILayoutY.h"
#include "GUI/BsCGUIWidget.h"
#include "GUI/BsGUIPanel.h"
#include "GUI/BsGUILayoutY.h"
#include "GUI/BsGUILabel.h"

namespace wind {
	bs::HSceneObject CreateCamera(bs::HSceneObject player)
	{
		using namespace bs;
		HSceneObject camera = SceneObject::create("Camera");
		camera->setParent(player);
		camera->setPosition(Vector3(0.0f, 1.8f * 0.5f - 0.1f, 0.0f));

		HCamera cameraComp = camera->addComponent<CCamera>();
		SPtr<RenderWindow> primaryWindow = gApplication().getPrimaryWindow();
		cameraComp->getViewport()->setTarget(primaryWindow);
		cameraComp->setMain(true);
		auto& windowProps = primaryWindow->getProperties();
		float aspectRatio = windowProps.width / (float)windowProps.height;
		cameraComp->setAspectRatio(aspectRatio);
		cameraComp->setProjectionType(PT_PERSPECTIVE);
		cameraComp->setHorzFOV(Degree(90));
		cameraComp->setNearClipDistance(0.005f);
		cameraComp->setFarClipDistance(1000.0f);

		HFPSCamera fpsCameraComp = camera->addComponent<FPSCamera>();
		fpsCameraComp->setCharacter(player);

		Cursor::instance().hide();
		Cursor::instance().clipToWindow(*primaryWindow);


		return camera;
	}

	bs::HSceneObject CreatePlayer()
	{
		using namespace bs;
		HSceneObject player = SceneObject::create("Player");
		HCharacterController charController = player->addComponent<CCharacterController>();
		charController->setHeight(1.0f);
		charController->setRadius(0.4f);
		player->addComponent<FPSWalker>();
		return player;
	}

	void SetupInput()
	{
		using namespace bs;
		gInput().onButtonUp.connect([](const ButtonEvent& ev) {
			if (ev.buttonCode == BC_ESCAPE) {
				gApplication().quitRequested();
			}
			});

		// Register input configuration
				// bsf allows you to use VirtualInput system which will map input device buttons and axes to arbitrary names,
				// which allows you to change input buttons without affecting the code that uses it, since the code is only
				// aware of the virtual names.  If you want more direct input, see Input class.
		auto inputConfig = gVirtualInput().getConfiguration();

		// Camera controls for buttons (digital 0-1 input, e.g. keyboard or gamepad button)
		inputConfig->registerButton("Forward", BC_W);
		inputConfig->registerButton("Back", BC_S);
		inputConfig->registerButton("Left", BC_A);
		inputConfig->registerButton("Right", BC_D);
		inputConfig->registerButton("Forward", BC_UP);
		inputConfig->registerButton("Back", BC_DOWN);
		inputConfig->registerButton("Left", BC_LEFT);
		inputConfig->registerButton("Right", BC_RIGHT);
		inputConfig->registerButton("FastMove", BC_LSHIFT);
		inputConfig->registerButton("RotateObj", BC_MOUSE_LEFT);
		inputConfig->registerButton("RotateCam", BC_MOUSE_RIGHT);
		inputConfig->registerButton("Space", BC_SPACE);

		// Camera controls for axes (analog input, e.g. mouse or gamepad thumbstick)
		// These return values in [-1.0, 1.0] range.
		inputConfig->registerAxis("Horizontal", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseX));
		inputConfig->registerAxis("Vertical", VIRTUAL_AXIS_DESC((UINT32)InputAxis::MouseY));
	}

	bs::HSceneObject CreateCube(bs::HMaterial material, bs::HPhysicsMaterial physicsMaterial) {
		using namespace bs;
		HSceneObject cube = SceneObject::create("Cube");
		HRenderable renderable = cube->addComponent<CRenderable>();
		HMesh mesh = gBuiltinResources().getMesh(BuiltinMesh::Box);
		renderable->setMesh(mesh);
		renderable->setMaterial(material);
		HBoxCollider boxCollider = cube->addComponent<CBoxCollider>();
		boxCollider->setMaterial(physicsMaterial);
		boxCollider->setMass(25.0f);
		HRigidbody boxRigidbody = cube->addComponent<CRigidbody>();
		return cube;
	}

	bs::HSceneObject CreateFloor(bs::HMaterial material, bs::HPhysicsMaterial physicsMaterial)
	{
		using namespace bs;
		HSceneObject floor = SceneObject::create("Floor");
		HRenderable floorRenderable = floor->addComponent<CRenderable>();
		HMesh planeMesh = gBuiltinResources().getMesh(BuiltinMesh::Quad);
		floorRenderable->setMaterial(material);
		floorRenderable->setMesh(planeMesh);
		HPlaneCollider planeCollider = floor->addComponent<CPlaneCollider>();
		planeCollider->setMaterial(physicsMaterial);
		floor->setScale(Vector3(50.0f, 1.0f, 50.0f));
		return floor;
	}

	bs::HPhysicsMaterial CreatePhysicsMaterial()
	{
		using namespace bs;
		HPhysicsMaterial physicsMaterial = PhysicsMaterial::create(1.0f, 1.0f, 0.5f);
		return physicsMaterial;
	}

	bs::HMaterial CreateMaterial(const bs::String& path)
	{
		using namespace bs;
		HShader shader = gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
		HMaterial material = Material::create(shader);
		HTexture texture = gImporter().import<Texture>(path);
		material->setTexture("gAlbedoTex", texture);
		return material;
	}

	bs::HSceneObject CreateGUI(bs::HSceneObject camera)
	{
		using namespace bs;
		auto gui = SceneObject::create("GUI");
		auto cameraComp = camera->getComponent<CCamera>();
		HGUIWidget guiComp = gui->addComponent<CGUIWidget>(cameraComp);
		GUIPanel* mainPanel = guiComp->getPanel();
		GUILayoutY* vertLayout = GUILayoutY::create();

		HString quitString(u8"Press the Escape key to quit");

		vertLayout->addNewElement<GUILabel>(quitString);
		mainPanel->addElement(vertLayout);
		return gui;
	}
}