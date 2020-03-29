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

#include "shader_compiler.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#undef max
#undef min
#include <SPIRV-Cross/spirv_glsl.hpp>
#include <shaderc/shaderc.hpp>

#include "shared/assert.hpp"
#include "shared/file/file_system.hpp"

// ========================================================================== //
// Private Functions
// ========================================================================== //

namespace wind {

static shaderc_shader_kind windShaderKindFromStage(ShaderStage stage) {
  switch (stage) {
  case ShaderStage::kVertex: {
    return shaderc_vertex_shader;
  }
  case ShaderStage::kPixel: {
    return shaderc_fragment_shader;
  }
  case ShaderStage::kCompute: {
    return shaderc_compute_shader;
  }
  default: {
    WN_PANIC("Invalid shader stage");
  }
  }
}

// -------------------------------------------------------------------------- //

static spv::ExecutionModel windSpvExeModelFromStage(ShaderStage stage) {
  switch (stage) {
  case ShaderStage::kVertex: {
    return spv::ExecutionModelVertex;
  }
  case ShaderStage::kPixel: {
    return spv::ExecutionModelFragment;
  }
  case ShaderStage::kCompute: {
    return spv::ExecutionModelGLCompute;
  }
  default: {
    WN_PANIC("Invalid shader stage");
  }
  }
}

} // namespace wind

// ========================================================================== //
// ShaderCompiler Implementation
// ========================================================================== //

namespace wind {

String ShaderCompiler::compile(ShaderStage stage, const String &source,
                               const String &main, const String &name,
                               const String &dir) {
  // Compile to SPIRV
  const shaderc::Compiler compiler;
  shaderc::CompileOptions opt;
  opt.SetSourceLanguage(shaderc_source_language_hlsl);

  const shaderc::SpvCompilationResult res = compiler.CompileGlslToSpv(
      source.getUTF8(), source.getSize(), windShaderKindFromStage(stage),
      name.getUTF8(), main.getUTF8(), opt);
  if (res.GetCompilationStatus() != shaderc_compilation_status_success) {
    const std::string errorMessage = res.GetErrorMessage();
    printf("Failed to compile shader: %s\n", errorMessage.c_str());
    return "";
  }

  // Compile to GLSL
  std::vector<u32> spv;
  spv.insert(spv.begin(), res.begin(), res.end());
  spirv_cross::CompilerGLSL xCompiler(spv);
  spirv_cross::CompilerGLSL::Options xOpt;
  xOpt.version = 450;
  xOpt.es = false;
  xCompiler.set_common_options(xOpt);
  xCompiler.build_combined_image_samplers();
  const std::string src = xCompiler.compile();

  return String(src);
}

// -------------------------------------------------------------------------- //

String ShaderCompiler::compile(ShaderStage stage, const Path &path,
                               const String &main) {
  const Path dir = path.parent();
  const String name = path.getName();
  const String source = FileSystem::readString(path);
  return compile(stage, source, main, name, dir.getString());
}

} // namespace wind
