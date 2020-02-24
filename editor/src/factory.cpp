// MIT License
//
// Copyright (c) 2020 Filip Björklund, Christoffer Gustafsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "factory.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "asset.hpp"
#include "log.hpp"

#include <BsCameraFlyer.h>
#include <BsPrerequisites.h>
#include <Components/BsCBoxCollider.h>
#include <Components/BsCCamera.h>
#include <Components/BsCMeshCollider.h>
#include <Components/BsCPlaneCollider.h>
#include <Components/BsCRenderable.h>
#include <Components/BsCSkybox.h>
#include <Material/BsMaterial.h>
#include <Physics/BsPhysicsMaterial.h>
#include <Resources/BsBuiltinResources.h>
#include <Scene/BsSceneManager.h>

// ========================================================================== //
// EditorFactory Implementation
// ========================================================================== //

namespace wind {

bs::HSceneObject EditorFactory::createEmptyScene(const bs::String &name,
                                                 f32 groundScale) {

  using namespace bs;

  const SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
  const RenderWindowProperties &windowProp = window->getProperties();

  // Root object
  HSceneObject root = SceneObject::create(name);

  // Camera and skybox
  {
    const HTexture skyboxTex = Asset::loadCubemap("res/skybox/daytime.hdr");
    HSceneObject skybox = SceneObject::create("skybox");
    skybox->setParent(root);
    HSkybox skyboxComp = skybox->addComponent<CSkybox>();
    skyboxComp->setTexture(skyboxTex);
  }

  /// Geometry
  {
    // Root
    HSceneObject geometry = SceneObject::create("geometry");
    geometry->setParent(root);

    // Load textures
    const HTexture texGrid = Asset::loadTexture("res/textures/grid.png");
    const HTexture texGrid2 = Asset::loadTexture("res/textures/grid_2.png");

    const HShader shader =
        gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);

    // Setup plane
    HMaterial planeMat = Material::create(shader);
    planeMat->setTexture("gAlbedoTex", texGrid);
    planeMat->setVec2("gUVTile", Vector2::ONE * groundScale * 4.0f);
    const HMesh planeMesh = gBuiltinResources().getMesh(BuiltinMesh::Quad);

    HSceneObject plane = SceneObject::create("geometry_plane");
    plane->setParent(geometry);
    plane->setScale(Vec3F(groundScale, 1.0f, groundScale));
    plane->move(Vec3F(groundScale, .4f, groundScale));
    HRenderable planeRenderable = plane->addComponent<CRenderable>();
    planeRenderable->setMesh(planeMesh);
    planeRenderable->setMaterial(planeMat);
    HPhysicsMaterial planePhysMat = PhysicsMaterial::create(1.0f, 1.0f, 0.5f);
    HPlaneCollider planeCollider = plane->addComponent<CPlaneCollider>();
    planeCollider->setMaterial(planePhysMat);
    planeCollider->setMass(0.0f);

#if 0
    // Box
    HMaterial boxMat = Material::create(shader);
    boxMat->setTexture("gAlbedoTex", texGrid2);
    const HMesh boxMesh = gBuiltinResources().getMesh(BuiltinMesh::Box);
    HSceneObject box = SceneObject::create("geometry_box");
    box->setParent(geometry);
    // box->setRotation(Quaternion(Degree(0), Degree(-55), Degree(0)));
    // box->setPosition(Vec3F(2.0f, 3.0f, 2.0f));
    box->setPosition(Vec3F(6.0f, 3.0f, 4.0f));
    box->setScale(Vec3F(1.0f, 20.0f, 40.0f));
    // box->setScale(Vec3F(1.0f, 10.0f, 1.5f));
    HRenderable boxRenderable = box->addComponent<CRenderable>();
    boxRenderable->setMesh(boxMesh);
    boxRenderable->setMaterial(boxMat);
    HPhysicsMaterial boxPhysMat = PhysicsMaterial::create(1.0f, 1.0f, 0.5f);
    HBoxCollider boxCollider = box->addComponent<CBoxCollider>();
    boxCollider->setMaterial(boxPhysMat);
    boxCollider->setMass(25.0f);
    // HRigidbody boxRigidbody = box->addComponent<CRigidbody>();
#endif
  }

  return root;
}

// -------------------------------------------------------------------------- //

bs::HSceneObject EditorFactory::createDefaultScene(const bs::String &name,
                                                   f32 groundScale) {
  using namespace bs;

  const SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
  const RenderWindowProperties &windowProp = window->getProperties();

  // Root object
  HSceneObject root = SceneObject::create(name);

  // Camera and skybox
  {
    const HTexture skyboxTex = Asset::loadCubemap("res/skybox/daytime.hdr");
    HSceneObject skybox = SceneObject::create("skybox");
    skybox->setParent(root);
    HSkybox skyboxComp = skybox->addComponent<CSkybox>();
    skyboxComp->setTexture(skyboxTex);
  }

  /// Geometry
  {
    // Root
    HSceneObject geometry = SceneObject::create("geometry");
    geometry->setParent(root);

    // Load textures
    const HTexture texGrid = Asset::loadTexture("res/textures/grid.png");
    const HTexture texGrid2 = Asset::loadTexture("res/textures/grid_2.png");

    const HShader shader =
        gBuiltinResources().getBuiltinShader(BuiltinShader::Standard);

    // Setup plane
    HMaterial planeMat = Material::create(shader);
    planeMat->setTexture("gAlbedoTex", texGrid);
    planeMat->setVec2("gUVTile", Vector2::ONE * groundScale * 4.0f);
    const HMesh planeMesh = gBuiltinResources().getMesh(BuiltinMesh::Quad);

    HSceneObject plane = SceneObject::create("geometry_plane");
    plane->setParent(geometry);
    plane->setScale(Vec3F(groundScale, 1.0f, groundScale));
    plane->move(Vec3F(groundScale, .4f, groundScale));
    HRenderable planeRenderable = plane->addComponent<CRenderable>();
    planeRenderable->setMesh(planeMesh);
    planeRenderable->setMaterial(planeMat);
    HPhysicsMaterial planePhysMat = PhysicsMaterial::create(1.0f, 1.0f, 0.5f);
    HPlaneCollider planeCollider = plane->addComponent<CPlaneCollider>();
    planeCollider->setMaterial(planePhysMat);
    planeCollider->setMass(0.0f);

#if 0
    // Box
    HMaterial boxMat = Material::create(shader);
    boxMat->setTexture("gAlbedoTex", texGrid2);
    const HMesh boxMesh = gBuiltinResources().getMesh(BuiltinMesh::Box);
    HSceneObject box = SceneObject::create("geometry_box");
    box->setParent(geometry);
    // box->setPosition(Vec3F(2.0f, 3.0f, 2.0f));
    box->setPosition(Vec3F(6.0f, 3.0f, 4.0f));
    box->setScale(Vec3F(1.0f, 10.0f, 1.5f));
    HRenderable boxRenderable = box->addComponent<CRenderable>();
    boxRenderable->setMesh(boxMesh);
    boxRenderable->setMaterial(boxMat);
    HPhysicsMaterial boxPhysMat = PhysicsMaterial::create(1.0f, 1.0f, 0.5f);
    HBoxCollider boxCollider = box->addComponent<CBoxCollider>();
    boxCollider->setMaterial(boxPhysMat);
    boxCollider->setMass(25.0f);
    // HRigidbody boxRigidbody = box->addComponent<CRigidbody>();
#endif

#if 1
    // Bunny
    auto [bunnyMesh, bunnyPhysMesh] =
        Asset::loadMeshWithPhysics("res/models/stanford-bunny.fbx", 0.006f);
    HMaterial bunnyMat = Material::create(shader);
    bunnyMat->setTexture("gAlbedoTex", texGrid2);
    HSceneObject bunny = SceneObject::create("geometry_bunny");
    bunny->setParent(geometry);
    bunny->setPosition(Vec3F(6.0f, 3.0f, 4.0f));
    HRenderable bunnyRenderable = bunny->addComponent<CRenderable>();
    bunnyRenderable->setMesh(bunnyMesh);
    bunnyRenderable->setMaterial(bunnyMat);
    HPhysicsMaterial bunnyPhysMat = PhysicsMaterial::create(1.0f, 1.0f, 1.0f);
    HMeshCollider bunnyCollider = bunny->addComponent<CMeshCollider>();
    bunnyCollider->setMaterial(bunnyPhysMat);
    bunnyCollider->setMass(15.0f);
    bunnyCollider->setMesh(bunnyPhysMesh);
#endif
  }

  return root;
}

} // namespace wind