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

#include "common.hpp"

#include <BsApplication.h>

// ========================================================================== //
// App Declaration
// ========================================================================== //

namespace wind {

/* Application class */
class App {
  friend class AppDelegate;

public:
  /* App information */
  struct Info {
    bs::String title;
    u32 width = 1280;
    u32 height = 720;
  };

  /* Video mode */
  struct VideoMode {
    u32 width;
    u32 height;
  };

public:
  /* Create application */
  App(const Info &info);

  /* Virtual destructor */
  virtual ~App() = default;

  /* Run application */
  void run();

  /* Hide application window */
  void hide();

  /* Show application window */
  void show();

  /* Enter fullscreen */
  void enterFullscreen(VideoMode videoMode = VideoMode{0, 0},
                       u32 monitorIdx = 0);

  /* Exit fullscreen */
  void exitFullscreen();

  /* Returns whether or not the application is currently fullscreen */
  bool isFullscreen() { return m_isFullscreen; }

  /* Show profiler overlay (GPU) */
  void showProfilerGPU(bs::HCamera camera);

  /* Show profiler overlay (CPU) */
  void showProfilerCPU(bs::HCamera camera);

  /* Hide profiler (GPU or CPU depending on which is active) */
  void hideProfiler();

  /* Startup callback */
  virtual void onStartup() {}

  /* Shutdown callback */
  virtual void onShutdown() {}

  /* Pre-update callback */
  virtual void onPreUpdate(f32 delta) {}

  /* Post-update callback */
  virtual void onPostUpdate(f32 delta) {}

  /* Fixed-update callback */
  virtual void onFixedUpdate(f32 delta) {}

private:
  static App *g_app;

private:
  /* Application title */
  bs::String m_title;
  /* Window width */
  u32 m_width;
  /* Window height */
  u32 m_height;

  /* Window width, pre-fullscreen */
  u32 m_widthPreFullscreen;
  /* Window height, pre-fullscreen */
  u32 m_heightPreFullscreen;

  /* Whether app is fullscreen */
  bool m_isFullscreen = false;
};

} // namespace wind
