#include "app.hpp"

namespace wind {

App::App(const bs::String& title, u32 w, u32 h) 
	:	m_title(title), m_width(w), m_height(h)
{}

void App::run() 
{
	using namespace bs;
	Application::startUp(VideoMode(m_width, m_height), m_title, false);
	Application::instance().runMainLoop();
	Application::shutDown();
}


}