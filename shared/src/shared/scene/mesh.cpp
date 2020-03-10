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

#include "mesh.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include <RenderAPI/BsVertexDataDesc.h>

// ========================================================================== //
// MeshManager Implementation
// ========================================================================== //

namespace wind {

bs::HMesh MeshManager::createSplineMesh(const Spline &spline) {
  // Create layout
  bs::SPtr<bs::VertexDataDesc> vertLayout = bs::VertexDataDesc::create();
  vertLayout->addVertElem(bs::VET_FLOAT3, bs::VES_POSITION);
  vertLayout->addVertElem(bs::VET_FLOAT3, bs::VES_NORMAL);

  // Create mesh
  bs::MESH_DESC meshDesc;
  meshDesc.numVertices = 4;
  meshDesc.numIndices = 6;
  meshDesc.vertexDesc = vertLayout;
  meshDesc.usage = bs::MU_STATIC;
  bs::HMesh mesh = bs::Mesh::create(meshDesc);

  // Upload data
  const bs::SPtr<bs::MeshData> data = mesh->allocBuffer();
  std::vector<Vec3F> positions;
  positions.resize(meshDesc.numVertices);
  std::vector<Vec3F> normals;
  normals.resize(meshDesc.numVertices);

  positions[0] = Vec3F(-0.5f, 1.0f, -0.5f);
  positions[1] = Vec3F(-0.5f, 1.0f, 0.5f);
  positions[2] = Vec3F(0.5f, 1.0f, 0.5f);
  positions[3] = Vec3F(0.5f, 1.0f, -0.5f);

  normals[0] = Vec3F(0.0f, 1.0f, 0.0f);
  normals[1] = Vec3F(0.0f, 1.0f, 0.0f);
  normals[2] = Vec3F(0.0f, 1.0f, 0.0f);
  normals[3] = Vec3F(0.0f, 1.0f, 0.0f);

  data->setVertexData(bs::VES_POSITION, positions.data(),
                      sizeof(Vec3F) * positions.size());
  data->setVertexData(bs::VES_NORMAL, normals.data(),
                      sizeof(Vec3F) * normals.size());

  u32 *indices = data->getIndices32();
  indices[0] = 0;
  indices[1] = 1;
  indices[2] = 2;
  indices[3] = 0;
  indices[4] = 2;
  indices[5] = 3;

  mesh->writeData(data, false);

  return mesh;
}

} // namespace wind
