#include "component_factory.hpp"

#include <Components/BsCBoxCollider.h>
#include <Components/BsCCharacterController.h>
#include <Components/BsCPlaneCollider.h>
#include <Components/BsCRenderable.h>
#include <Importer/BsImporter.h>
#include <Material/BsMaterial.h>
#include <Physics/BsPhysicsMaterial.h>
#include <Resources/BsBuiltinResources.h>

#include "shared/asset.hpp"
#include "shared/log.hpp"
#include "shared/scene/cnet_component.hpp"
#include "shared/scene/crotor.hpp"
#include "shared/scene/wind_src.hpp"
#include "shared/state/moveable_state.hpp"

namespace wind {

ComponentFactory::ComponentFactory() {
  using namespace bs;
  m_matGrid = material("res/textures/grid.png");
  m_matGrid2 = material("res/textures/grid_2.png");
  m_matWireframe = transparentMaterial("res/textures/wireframe.png");
  m_physicsMatStd = physicsMaterial();
  m_meshBall = gBuiltinResources().getMesh(BuiltinMesh::Sphere);
  m_meshCube = gBuiltinResources().getMesh(BuiltinMesh::Box);
  m_meshRotor = gImporter().import<Mesh>("res/meshes/rotor.fbx");
}

HCNetComponent
ComponentFactory::create(const MoveableState &moveableState) const {
  switch (moveableState.getType()) {
  case ComponentTypes::kPlayer:
    logError("[ComponentFactory] called create with a player, but I cannot "
             "create players.");
    break;
  case ComponentTypes::kCube:
    return cube(moveableState);
  case ComponentTypes::kBall:
    return ball(moveableState);
  case ComponentTypes::kRotor:
    return rotor(moveableState);
  case ComponentTypes::kWindSource:
    return windSource(moveableState);
  default:
    logWarning(
        "[ComponentFactory] create called with invalid type. type {}, id {}",
        static_cast<u32>(moveableState.getType()),
        moveableState.getUniqueId().raw());
  }
  return HCNetComponent{};
}

HCNetComponent
ComponentFactory::cube(const MoveableState &moveableState) const {
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
  netComp->setType(ComponentTypes::kCube);
  return netComp;
}

HCNetComponent
ComponentFactory::ball(const MoveableState &moveableState) const {
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
  netComp->setType(ComponentTypes::kBall);
  return netComp;
}

HCNetComponent
ComponentFactory::rotor(const MoveableState &moveableState) const {
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
  collider->setExtents(Vector3(18.0f, 1.0f, 18.0f));
  HRigidbody rigid = so->addComponent<CRigidbody>();
  rigid->setUseGravity(false);
  rigid->setIsKinematic(true);

  // HSceneObject wind = SceneObject::create("WindSource");
  // wind->setParent(so);
  // auto windSource = wind->addComponent<CWindSource>(m_matWireframe,
  // m_meshCube); windSource->addFunction(BaseFn::fnConstant(bs::Vector3{1.0f,
  // -500.0f, 1.0f})); wind->setScale(Vector3(5.0f, 10.0f, 5.0f) * 10.0f);
  // wind->setPosition(Vector3())

  auto netComp = so->addComponent<CNetComponent>(moveableState);
  netComp->setType(ComponentTypes::kRotor);
  auto crotor = so->addComponent<CRotor>();
  crotor->setRotation(5.0f);

  return netComp;
}

HCNetComponent
ComponentFactory::windSource(const MoveableState &moveableState) const {
  using namespace bs;
  logVerbose("creating a windSource {}", moveableState.getUniqueId().raw());
  HSceneObject wind = SceneObject::create("WindSource");
  auto windSource = wind->addComponent<CWindSource>(m_matWireframe, m_meshCube);
  windSource->addFunction(BaseFn::fnConstant(bs::Vector3{1.0f, 100.0f, 1.0f}));
  auto netComp = wind->addComponent<CNetComponent>(moveableState);
  netComp->setType(ComponentTypes::kWindSource);
  wind->setScale(Vector3(2.0f, 5.0f, 5.0f));
  return netComp;
}

void ComponentFactory::floor() const {
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

bs::HPhysicsMaterial ComponentFactory::physicsMaterial() {
  using namespace bs;
  HPhysicsMaterial physicsMaterial = PhysicsMaterial::create(1.0f, 1.0f, 0.5f);
  return physicsMaterial;
}

bs::HMaterial ComponentFactory::material(const bs::String &path) {
  using namespace bs;
  HShader shader =
      gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);
  HMaterial material = Material::create(shader);
  HTexture texture = Asset::loadTexture(path);
  material->setTexture("gAlbedoTex", texture);
  return material;
}

bs::HMaterial ComponentFactory::transparentMaterial(const bs::String &path) {
  using namespace bs;
  HShader shader =
      gBuiltinResources().getBuiltinShader(BuiltinShader::Transparent);
  HMaterial material = Material::create(shader);
  HTexture texture = Asset::loadTexture(path);
  material->setTexture("gAlbedoTex", texture);
  return material;
}

} // namespace wind
