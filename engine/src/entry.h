#pragma once

#include "defines.h"
#include "application.h"
#include "log.h"

#ifdef KPLATFORM_WINDOWS

extern bve::Application* bve::CreateApplication();

int main(int argc, char** argv) 
{
	bve::Log::init();

	auto app = bve::CreateApplication();
	app->run();
	delete app;
}

#endif