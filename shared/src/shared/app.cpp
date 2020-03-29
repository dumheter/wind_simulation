// MIT License
//
// Copyright (c) 2020 Filip Bj�rklund, Christoffer Gustafsson
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

#include "app.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

// ========================================================================== //
// Headers
// ========================================================================== //

namespace wind {

App::App(const Info &info)
    : m_title(info.title), m_width(info.width), m_height(info.height) {
  if (!glfwInit()) {
  }

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
  m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
  // Assert()
  glfwMakeContextCurrent(m_window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
  }
}

// -------------------------------------------------------------------------- //

App::~App() {
  //
  glfwDestroyWindow(m_window);
}

// -------------------------------------------------------------------------- //

void App::run() {
  m_running = true;

  // Run main-loop
  while (m_running) {
    glfwPollEvents();

    update(0.0f);
    fixedUpdate(0.0f);
    render();

    // Check closing
    if (glfwWindowShouldClose(m_window)) {
      m_running = false;
    }
  }
}

} // namespace wind
