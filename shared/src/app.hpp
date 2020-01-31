#pragma once

#include "BsApplication.h"


#include "common.hpp"

namespace wind {

class App 
{
public:
	/* App information */
	struct Info {
		bs::String title;
		u32 width;
		u32 height;
	};

public:
	/* Create application */
	App(const Info& info);

	/* Run application */
	void run();

	/* Hide application window */
	void hide();

	/* Show application window */
	void show();

private:
	/* Application title */
	bs::String m_title;
	u32 m_width;
	u32 m_height;

};

}