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

#include "common.hpp"

#include <BsApplication.h>

// ========================================================================== //
// App Declaration
// ========================================================================== //

namespace wind
{

/* Application class */
class App
{
    friend class AppDelegate;

  public:
    /* App information */
    struct Info
    {
        bs::String title;
        u32 width = 1280;
        u32 height = 720;
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

    /* Show profiler overlay (GPU) */
    void showProfilerGPU(bs::HCamera camera);

    /* Show profiler overlay (CPU) */
    void showProfilerCPU(bs::HCamera camera);

    /* Hide profiler (GPU or CPU depending on which is active) */
    void hideProfiler();

    /* Startup callback */
    virtual void OnStartup()
    {
    }

    /* Shutdown callback */
    virtual void OnShutdown()
    {
    }
    /* Pre-update callback */
    virtual void OnPreUpdate()
    {
    }

    /* Post-update callback */
    virtual void OnPostUpdate()
    {
    }

    /* Fixed-update callback */
    virtual void OnFixedUpdate()
    {
    }

  private:
    static App *g_app;

  private:
    /* Application title */
    bs::String m_title;
    u32 m_width;
    u32 m_height;
};

} // namespace wind
