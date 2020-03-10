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

#include "shader.hpp"

#include <Importer/BsImporter.h>
#include <Material/BsPass.h>
#include <Resources/BsResources.h>

#include "shared/utility/util.hpp"

// ========================================================================== //
// Static data
// ========================================================================== //

static const char sWireframeVS[] = "struct VsIn {\n"
                                   "  float3 pos : POSITION;\n"
                                   "}\n"
                                   "\n"
                                   "struct VsOut {\n"
                                   "  float4 pos;\n"
                                   "}\n"
                                   "\n"
                                   "cbuffer Transform : b0 {\n"
                                   "  float4x4 mvp;\n"
                                   "}\n"
                                   "\n"
                                   "VsOut main(VsIn in) {\n"
                                   "  VsOut out;\n"
                                   "  out.pos = mul(mvp, in.pos);\n"
                                   "  return out;\n"
                                   "}";

// -------------------------------------------------------------------------- //

static const char sWireframePS[] = "struct VsOut {\n"
                                   "  float4 pos;\n"
                                   "}\n"
                                   "\n"
                                   "float4 main(VsOut in) {\n"
                                   " return float4(1.0f, 0.0f, 0.0f, 1.0f);\n"
                                   "}\n";

// ========================================================================== //
// ShaderManager Implementation
// ========================================================================== //

namespace wind {

ShaderManager::ShaderManager() {
  //
  m_shaderWireframe = loadWireframe();
}

// -------------------------------------------------------------------------- //

bs::HShader ShaderManager::loadWireframe() {
  bs::HShader shader =
      bs::gImporter().import<bs::Shader>("res/shaders/wireframe.bsl");
  return shader;
}

} // namespace wind
