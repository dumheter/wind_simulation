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

#include "app.hpp"

// ========================================================================== //
// Headers
// ========================================================================== //

#include "scene/component/cpaint.hpp"

#include "microprofile/microprofile.h"

#include <Components/BsCCamera.h>
#include <RenderAPI/BsRenderAPI.h>
#include <RenderAPI/BsRenderWindow.h>
#include <Renderer/BsCamera.h>
#include <Scene/BsSceneManager.h>
#include <Scene/BsSceneObject.h>
#include <Utility/BsTime.h>

// ========================================================================== //
// AppDelegate Declaration/Implementation
// ========================================================================== //

namespace wind {

/* Delegate for application events */
class AppDelegate : public bs::Application {
public:
  explicit AppDelegate(const bs::START_UP_DESC &desc) : Application(desc) {}

protected:
  void fixedUpdate() override {
    MICROPROFILE_SCOPEI("App", "onFixedUpdate", MP_GREEN);
    Application::fixedUpdate();
    const f32 delta = bs::gTime().getFixedFrameDelta();
    App::g_app->onFixedUpdate(delta);

    if (App::g_app->m_tpsSkip-- == 0) {
      App::g_app->m_tpsSkip = 60 / App::g_app->m_tps;
      App::g_app->onTick();
    }
  }

  void onStartUp() override {
    MICROPROFILE_SCOPEI("App", "onStartup", MP_GREEN);
    Application::onStartUp();
  }

  void onShutDown() override {
    MICROPROFILE_SCOPEI("App", "onShutdown", MP_GREEN);
    Application::onShutDown();
    App::g_app->onShutdown();
  }

  void preUpdate() override {
    MicroProfileFlip(nullptr);
    MICROPROFILE_SCOPEI("App", "onPreUpdate", MP_GREEN);
    Application::preUpdate();
    const f32 delta = bs::gTime().getFrameDelta();
    App::g_app->onPreUpdate(delta);
  }

  void postUpdate() override {
    MICROPROFILE_SCOPEI("App", "onPostUpdate", MP_GREEN);
    Application::postUpdate();
    const f32 delta = bs::gTime().getFrameDelta();
    App::g_app->onPostUpdate(delta);
    App::g_app->paint();
  }
};

} // namespace wind

// ========================================================================== //
// App Implementation
// ========================================================================== //

namespace wind {

App *App::g_app = nullptr;

// -------------------------------------------------------------------------- //

App::App(const Info &info)
    : m_title(info.title), m_width(info.width), m_height(info.height),
      m_tps(info.tps), m_widthPreFullscreen(info.width),
      m_heightPreFullscreen(info.height) {
  using namespace bs;

  g_app = this;

  // Create application
  START_UP_DESC appDesc;
  appDesc.renderAPI = "bsfD3D11RenderAPI";
  appDesc.renderer = "bsfRenderBeast";
  appDesc.physics = "bsfPhysX";
  appDesc.audio = "bsfOpenAudio";
  appDesc.importers = {"bsfFBXImporter", "bsfFontImporter",
                       "bsfFreeImgImporter", "bsfSL"};
  appDesc.primaryWindowDesc.title = m_title;
  appDesc.primaryWindowDesc.hidden = true;
  appDesc.primaryWindowDesc.allowResize = true;
  appDesc.primaryWindowDesc.videoMode = bs::VideoMode(m_width, m_height);
  appDesc.primaryWindowDesc.vsync = true;
  Application::startUp<AppDelegate>(appDesc);

  // Connect resize callback
  Application::instance().getPrimaryWindow()->onResized.connect([=]() {
    SPtr<RenderWindow> window = Application::instance().getPrimaryWindow();
    m_width = window->getProperties().width;
    m_height = window->getProperties().height;
    g_app->onWindowResize();
  });

  // Initialize MicroProfile
  MicroProfileInit();
}

// -------------------------------------------------------------------------- //

void App::run() {
  using namespace bs;

  onStartup();

  show();
  Application::instance().runMainLoop();
  Application::shutDown();
}

// -------------------------------------------------------------------------- //

void App::hide() {
  using namespace bs;
  SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
  window->hide();
}

// -------------------------------------------------------------------------- //

void App::enterFullscreen(VideoMode videoMode, u32 monitorIdx) {
  bs::SPtr<bs::RenderWindow> window =
      bs::Application::instance().getPrimaryWindow();
  m_widthPreFullscreen = window->getProperties().width;
  m_heightPreFullscreen = window->getProperties().height;

  bs::VideoMode bsVideoMode(videoMode.width, videoMode.height);

  // Make sure the monitor index is valid
  const bs::VideoModeInfo &vmi = bs::RenderAPI::getVideoModeInfo();
  if (monitorIdx >= vmi.getNumOutputs()) {
    monitorIdx = 0;
    // TODO(Filip) Add logging warning
  }

  // Find if the requested mode exists
  bool found = false;
  const bs::VideoOutputInfo &voi = vmi.getOutputInfo(monitorIdx);
  for (u32 j = 0; j < voi.getNumVideoModes(); j++) {
    const bs::VideoMode &vm = voi.getVideoMode(j);
    if (vm == bsVideoMode) {
      found = true;
      break;
    }
  }

  // Otherwise pick one
  if (!found) {
    const bs::VideoOutputInfo &voi = vmi.getOutputInfo(monitorIdx);
    bsVideoMode = voi.getVideoMode(voi.getNumVideoModes() - 1);
  }

  window->setFullscreen(bsVideoMode);
  m_isFullscreen = true;
}

// -------------------------------------------------------------------------- //

void App::exitFullscreen() {
  bs::SPtr<bs::RenderWindow> window =
      bs::Application::instance().getPrimaryWindow();
  window->setWindowed(m_widthPreFullscreen, m_heightPreFullscreen);
  m_isFullscreen = false;
}

// -------------------------------------------------------------------------- //

void App::show() {
  using namespace bs;
  SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
  window->show();
}

// -------------------------------------------------------------------------- //

void App::quit() { bs::gApplication().quitRequested(); }

// -------------------------------------------------------------------------- //

void App::showProfilerGPU(bs::HCamera camera) {
  using namespace bs;

  const SPtr<Camera> c = camera.get()->_getCamera();
  gApplication().showProfilerOverlay(ProfilerOverlayType::GPUSamples, c);
}

// -------------------------------------------------------------------------- //

void App::showProfilerCPU(bs::HCamera camera) {
  using namespace bs;

  const SPtr<Camera> c = camera.get()->_getCamera();
  gApplication().showProfilerOverlay(ProfilerOverlayType::CPUSamples, c);
}

// -------------------------------------------------------------------------- //

void App::hideProfiler() {
  using namespace bs;

  gApplication().hideProfilerOverlay();
}

// -------------------------------------------------------------------------- //

void App::paint() {
  m_painter.begin();

  /// Let all CPaint components paint
  std::vector<CPaint *> objects = CPaint::getAll();
  for (auto &object : objects) {
    object->paint(m_painter);
  }

  /// Paint self
  onPaint(m_painter);

  m_painter.end();
}

// -------------------------------------------------------------------------- //

App::Info App::makeInfo(const bs::String &title, u32 width, u32 height,
                        u32 tps) {
  return Info{title, width, height, tps};
}

} // namespace wind
