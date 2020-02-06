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

#include "Components/BsCCamera.h"
#include "RenderAPI/BsRenderWindow.h"
#include "Renderer/BsCamera.h"
#include "microprofile/microprofile.h"

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
    MICROPROFILE_SCOPEI("group", "timername", MP_YELLOW);
    Application::fixedUpdate();
    App::g_app->OnFixedUpdate();
  }

  void onStartUp() override {
    Application::onStartUp();
    App::g_app->OnStartup();
  }

  void onShutDown() override {
    Application::onShutDown();
    App::g_app->OnShutdown();
  }

  void preUpdate() override {
    Application::preUpdate();
    App::g_app->OnPreUpdate();
  }

  void postUpdate() override {
    Application::postUpdate();
    App::g_app->OnPostUpdate();
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
    : m_title(info.title), m_width(info.width), m_height(info.height) {
  using namespace bs;

  g_app = this;

  START_UP_DESC appDesc;
  appDesc.renderAPI = "bsfD3D11RenderAPI";
  appDesc.renderer = "bsfRenderBeast";
  appDesc.physics = "bsfPhysX";
  appDesc.audio = "bsfOpenAudio";
  appDesc.importers = {"bsfFBXImporter", "bsfFontImporter",
                       "bsfFreeImgImporter"};
  appDesc.primaryWindowDesc.title = m_title;
  appDesc.primaryWindowDesc.hidden = true;
  appDesc.primaryWindowDesc.allowResize = true;
  appDesc.primaryWindowDesc.videoMode = VideoMode(m_width, m_height);
  Application::startUp<AppDelegate>(appDesc);
}

// -------------------------------------------------------------------------- //

void App::run() {
  using namespace bs;

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

void App::show() {
  using namespace bs;
  SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
  window->show();
}

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

} // namespace wind
