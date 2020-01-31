#include "app.hpp"

#include "RenderAPI/BsRenderWindow.h"

namespace wind {

App::App(const Info& info)
	: m_title(info.title), m_width(info.width), m_height(info.height)
{
	using namespace bs;

	START_UP_DESC appDesc;
	appDesc.renderAPI = "bsfD3D11RenderAPI";
	appDesc.renderer = "bsfRenderBeast";
	appDesc.physics = "bsfPhysX";
	appDesc.audio = "bsfOpenAudio";
	appDesc.importers = { "bsfFBXImporter", "bsfFontImporter", "bsfFreeImgImporter" };
	appDesc.primaryWindowDesc.title = m_title;
	appDesc.primaryWindowDesc.hidden = true;
	appDesc.primaryWindowDesc.allowResize = true;
	appDesc.primaryWindowDesc.videoMode = VideoMode(m_width, m_height);
	Application::startUp(appDesc);
}

void App::run()
{
	using namespace bs;

	show();
	Application::instance().runMainLoop();
	Application::shutDown();
}

void App::hide()
{
	using namespace bs;
	SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
	window->hide();
}

void App::show()
{
	using namespace bs;
	SPtr<RenderWindow> window = gApplication().getPrimaryWindow();
	window->show();
}


}