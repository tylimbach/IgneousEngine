#pragma once

#include "defines.h"
#include "application.h"
#include "log.h"

extern bve::Application* bve::createApplication();

int main(int argc, char** argv) 
{
	bve::Log::init();

	auto app = bve::createApplication();
	app->run();
	delete app;
}
