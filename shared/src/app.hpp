#pragma once

#include "BsApplication.h"

#include "common.hpp"

namespace wind {

class App 
{
public:
	App(const bs::String& title, u32 w, u32 h);

	void run();

private:
	/* Application title */
	bs::String m_title;
	u32 m_width;
	u32 m_height;

};

}