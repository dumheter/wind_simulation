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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
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

#include "app.hpp"

// ========================================================================== //
// Editor Declaration
// ========================================================================== //

namespace wind
{

/* Main Editor class */
class Editor : public App
{
  public:
    Editor();

    void OnPreUpdate() override;

  private:
    /* Setup the camera */
    void setupCamera();

    /* Setup the editor scene */
    void setupScene();

    /* Setup the editor GUI */
    void setupGUI();

    /* Register controls */
    void registerControls();

  public:
    /* Editor window width */
    static constexpr u32 WINDOW_WIDTH = 1280;
    /* Editor window height */
    static constexpr u32 WINDOW_HEIGHT = 720;

    /* Scale of the ground plane */
    static constexpr f32 GROUND_PLANE_SCALE = 15.0f;
};

} // namespace wind