#include "creator.hpp"
#include "BsApplication.h"
#include "BsFPSCamera.h"
#include "Components/BsCBoxCollider.h"
#include "Components/BsCCamera.h"
#include "Components/BsCCharacterController.h"
#include "Components/BsCMeshCollider.h"
#include "Components/BsCPlaneCollider.h"
#include "Components/BsCRenderable.h"
#include "Components/BsCRigidbody.h"
#include "Components/BsCSphereCollider.h"
#include "GUI/BsCGUIWidget.h"
#include "GUI/BsGUIButton.h"
#include "GUI/BsGUIContent.h"
#include "GUI/BsGUIInputBox.h"
#include "GUI/BsGUILabel.h"
#include "GUI/BsGUILayout.h"
#include "GUI/BsGUILayoutX.h"
#include "GUI/BsGUILayoutY.h"
#include "GUI/BsGUIPanel.h"
#include "GUI/BsGUIViewport.h"
#include "Importer/BsImporter.h"
#include "Input/BsInput.h"
#include "Material/BsMaterial.h"
#include "Mesh/BsMesh.h"
#include "Physics/BsBoxCollider.h"
#include "Physics/BsMeshCollider.h"
#include "Physics/BsPhysicsMaterial.h"
#include "Physics/BsPhysicsMesh.h"
#include "Physics/BsPlaneCollider.h"
#include "Physics/BsRigidbody.h"
#include "Platform/BsCursor.h"
#include "RenderAPI/BsRenderTarget.h"
#include "RenderAPI/BsRenderWindow.h"
#include "Resources/BsBuiltinResources.h"
#include "Scene/BsSceneObject.h"
#include "asset.hpp"
#include "bsFPSWalker.h"
#include "cmyplayer.hpp"
#include "log.hpp"
#include "moveable_state.hpp"
#include "utility/util.hpp"
#include "world.hpp"
#include <Components/BsCSkybox.h>
#include <alflib/core/assert.hpp>
#include <chrono>
#include <cstdlib>
#include "crotor.hpp"

namespace wind {

Creator::Creator(World *world) : m_world(world) {
  using namespace bs;
  m_matGrid = material("res/textures/grid.png");
  m_matGrid2 = material("res/textures/grid_2.png");
  m_physicsMatStd = physicsMaterial();
  m_meshBall = gBuiltinResources().getMesh(BuiltinMesh::Sphere);
  m_meshCube = gBuiltinResources().getMesh(BuiltinMesh::Box);
  m_meshRotor = gImporter().import<Mesh>("res/meshes/rotor.fbx");
}

HCNetComponent Creator::create(const MoveableState &moveableState) const {
  switch (moveableState.getType()) {
  case Types::kPlayer:
    return player(moveableState);
  case Types::kCube:
    return cube(moveableState);
  case Types::kBall:
    return ball(moveableState);
  case Types::kRotor:
    return rotor(moveableState);
  default:
    logWarning("[creator] create called with invalid type. type {}, id {}",
               static_cast<u32>(moveableState.getType()),
               moveableState.getUniqueId().raw());
  }
  return HCNetComponent{};
}

HCNetComponent Creator::player(const MoveableState &moveableState) const {
  using namespace bs;
  logVerbose("creating a player {}", moveableState.getUniqueId().raw());
  HSceneObject player = SceneObject::create("Player");

  HCharacterController charController =
      player->addComponent<CCharacterController>();
  charController->setHeight(1.0f);
  charController->setRadius(0.4f);
  auto prep = SceneObject::create("playerRep");
  prep->setParent(player);
  HRenderable renderable = prep->addComponent<CRenderable>();
  prep->setScale(Vector3(0.3f, 1.8f, 0.3f));
  prep->setPosition(Vector3(0.0f, -1.0f, 0.0f));
  HMesh mesh = gBuiltinResources().getMesh(BuiltinMesh::Cylinder);
  renderable->setMesh(mesh);
  renderable->setMaterial(m_matGrid2);
  auto fpsWalker = player->addComponent<FPSWalker>(m_world);
  HCNetComponent netComp = player->addComponent<CNetComponent>(moveableState);
  netComp->setType(Types::kPlayer);
  auto [it, ok] =
      m_world->getNetComps().insert({netComp->getUniqueId(), netComp});
  AlfAssert(ok, "failed to add player");
  auto [it2, ok2] =
      m_world->getWalkers().insert({netComp->getUniqueId(), fpsWalker});
  AlfAssert(ok2, "failed to add player");
  return netComp;
}

HCNetComponent Creator::cube(const MoveableState &moveableState) const {
  using namespace bs;
  logVerbose("creating a cube {}", moveableState.getUniqueId().raw());
  HSceneObject cube = SceneObject::create("Cube");
  HRenderable renderable = cube->addComponent<CRenderable>();
  renderable->setMesh(m_meshCube);
  renderable->setMaterial(m_matGrid2);
  HBoxCollider boxCollider = cube->addComponent<CBoxCollider>();
  boxCollider->setMaterial(m_physicsMatStd);
  boxCollider->setMass(40.0f);
  HRigidbody boxRigidbody = cube->addComponent<CRigidbody>();
  boxRigidbody->setSleepThreshold(0.1f);
  auto netComp = cube->addComponent<CNetComponent>(moveableState);
  netComp->setType(Types::kCube);
  auto [it, ok] =
      m_world->getNetComps().insert({moveableState.getUniqueId(), netComp});
  AlfAssert(ok, "failed to create cube, was the id unique?");
  return netComp;
}

HCNetComponent Creator::ball(const MoveableState &moveableState) const {
  using namespace bs;
  logVerbose("creating a ball {}", moveableState.getUniqueId().raw());
  HSceneObject sphere = SceneObject::create("Sphere");
  sphere->setScale(Vector3(0.3f, 0.3f, 0.3f));
  HRenderable renderable = sphere->addComponent<CRenderable>();
  renderable->setMesh(m_meshCube);
  renderable->setMaterial(m_matGrid2);
  auto collider = sphere->addComponent<CBoxCollider>();
  collider->setMaterial(m_physicsMatStd);
  collider->setMass(8.0f);
  HRigidbody rigid = sphere->addComponent<CRigidbody>();
  rigid->setSleepThreshold(0.1f);
  auto netComp = sphere->addComponent<CNetComponent>(moveableState);
  netComp->setType(Types::kBall);
  auto [it, ok] =
      m_world->getNetComps().insert({moveableState.getUniqueId(), netComp});
  AlfAssert(ok, "failed to create ball, was the id unique?");
  return netComp;
}

HCNetComponent Creator::rotor(const MoveableState &moveableState) const {
  using namespace bs;
  logVerbose("creating a rotor {}", moveableState.getUniqueId().raw());
  HSceneObject so = SceneObject::create("Rotor");
  so->setScale(Vector3(0.1f, 0.1f, 0.1f));
  HRenderable renderable = so->addComponent<CRenderable>();
  renderable->setMesh(m_meshRotor);
  renderable->setMaterial(m_matGrid2);
  auto collider = so->addComponent<CBoxCollider>();
  collider->setMaterial(m_physicsMatStd);
  collider->setMass(20.0f);
  collider->setExtents(Vector3(19.0f, 1.0f, 19.0f));
  HRigidbody rigid = so->addComponent<CRigidbody>();
  //rigid->setSleepThreshold(0.1f);
  rigid->setUseGravity(false);
  rigid->setIsKinematic(true);
  auto netComp = so->addComponent<CNetComponent>(moveableState);
  netComp->setType(Types::kRotor);
  auto crotor = so->addComponent<CRotor>();
  crotor->setRotation(5.0f);
    auto [it, ok] =
      m_world->getNetComps().insert({moveableState.getUniqueId(), netComp});
  AlfAssert(ok, "failed to create rotor, was the id unique?");
  return netComp;
}

void Creator::floor() const {
  using namespace bs;
  HSceneObject floor = SceneObject::create("Floor");
  HRenderable floorRenderable = floor->addComponent<CRenderable>();
  HMesh planeMesh = gBuiltinResources().getMesh(BuiltinMesh::Quad);
  floorRenderable->setMaterial(m_matGrid);
  floorRenderable->setMesh(planeMesh);
  HPlaneCollider planeCollider = floor->addComponent<CPlaneCollider>();
  planeCollider->setMaterial(m_physicsMatStd);
  constexpr float kSize = 50.0f;
  floor->setScale(Vector3(kSize, 1.0f, kSize));
  m_matGrid->setVec2("gUVTile", Vector2::ONE - (Vector2::ONE * -kSize));
}

bs::HPhysicsMaterial Creator::physicsMaterial() {
  using namespace bs;
  HPhysicsMaterial physicsMaterial = PhysicsMaterial::create(1.0f, 1.0f, 0.5f);
  return physicsMaterial;
}

bs::HMaterial Creator::material(const bs::String &path) {
  using namespace bs;
  HShader shader =
      gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
  HMaterial material = Material::create(shader);
  HTexture texture = Asset::loadTexture(path);
  material->setTexture("gAlbedoTex", texture);
  return material;
}
} // namespace wind
