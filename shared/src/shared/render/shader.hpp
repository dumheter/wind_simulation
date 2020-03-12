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

#pragma once

// ========================================================================== //
// Headers
// ========================================================================== //

#include <Material/BsShader.h>

// ========================================================================== //
// ShaderManager Declaration
// ========================================================================== //

namespace wind {

class ShaderManager {
public:
  /// Returns the diffuse shader
  static bs::HShader getDiffuse() { return instance().m_shaderDiffuse; }

  /// Returns the transparent shader
  static bs::HShader getTransparent() { return instance().m_shaderTransparent; }

  /// Returns the transparent shader with back-face culling disabled
  static bs::HShader getTransparentNoCull() {
    return instance().m_shaderTransparentNoCull;
  }

  /// Returns the wireframe shader
  static bs::HShader getWireframe() { return instance().m_shaderWireframe; }

private:
  /// Construct shader manager
  ShaderManager();

  /// Returns the static instance of the Shader class
  static ShaderManager &instance() {
    static ShaderManager shader;
    return shader;
  }

  /// Diffuse shader
  bs::HShader m_shaderDiffuse;
  /// Transparent shader
  bs::HShader m_shaderTransparent;
  /// Transparent shader (No back-face culling)
  bs::HShader m_shaderTransparentNoCull;
  /// Wireframe shader
  bs::HShader m_shaderWireframe;
};

} // namespace wind