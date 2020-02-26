#include "creator.hpp"
#include "Components/BsCBoxCollider.h"
#include "Components/BsCCharacterController.h"
#include "Components/BsCPlaneCollider.h"
#include "Components/BsCRenderable.h"
#include "Importer/BsImporter.h"
#include "Material/BsMaterial.h"
#include "Physics/BsPhysicsMaterial.h"
#include "Resources/BsBuiltinResources.h"
#include "asset.hpp"
#include "log.hpp"
#include "scene/component_factory.hpp"
#include "world.hpp"

namespace wind {

Creator::Creator(World *world) : m_world(world), m_factory() {}

HCNetComponent Creator::create(const MoveableState &moveableState) const {
  switch (moveableState.getType()) {
  case ComponentTypes::kPlayer:
    return player(moveableState);
  case ComponentTypes::kCube:
    return cube(moveableState);
  case ComponentTypes::kBall:
    return ball(moveableState);
  case ComponentTypes::kRotor:
    return rotor(moveableState);
  case ComponentTypes::kWindSource:
    return windSource(moveableState);
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
  renderable->setMaterial(m_factory.m_matGrid2);
  auto fpsWalker = player->addComponent<FPSWalker>(m_world);
  HCNetComponent netComp = player->addComponent<CNetComponent>(moveableState);
  netComp->setType(ComponentTypes::kPlayer);
  auto [it, ok] =
      m_world->getNetComps().insert({netComp->getUniqueId(), netComp});
  AlfAssert(ok, "failed to add player");
  auto [it2, ok2] =
      m_world->getWalkers().insert({netComp->getUniqueId(), fpsWalker});
  AlfAssert(ok2, "failed to add player");
  return netComp;
}

HCNetComponent Creator::cube(const MoveableState &moveableState) const {
  auto netComp = m_factory.cube(moveableState);
  auto [it, ok] =
      m_world->getNetComps().insert({moveableState.getUniqueId(), netComp});
  AlfAssert(ok, "failed to create cube, was the id unique?");
  return netComp;
}

HCNetComponent Creator::ball(const MoveableState &moveableState) const {
  auto netComp = m_factory.ball(moveableState);
  netComp->setType(ComponentTypes::kBall);
  auto [it, ok] =
      m_world->getNetComps().insert({moveableState.getUniqueId(), netComp});
  AlfAssert(ok, "failed to create ball, was the id unique?");
  return netComp;
}

HCNetComponent Creator::rotor(const MoveableState &moveableState) const {
  auto netComp = m_factory.rotor(moveableState);
  auto [it, ok] =
      m_world->getNetComps().insert({moveableState.getUniqueId(), netComp});
  AlfAssert(ok, "failed to create rotor, was the id unique?");
  return netComp;
}

HCNetComponent Creator::windSource(const MoveableState &moveableState) const {
  auto netComp = m_factory.windSource(moveableState);
  auto [it, ok] =
      m_world->getNetComps().insert({moveableState.getUniqueId(), netComp});
  AlfAssert(ok, "failed to create windSource, was the id unique?");
  return netComp;
}

void Creator::floor() const { m_factory.floor(); }

} // namespace wind
