#include <engine.h>
#include "lua.hpp"

#include <iostream>

class ExampleApp : public bve::Application
{
public:
	ExampleApp() {};
	~ExampleApp() {};
};

bve::Application* bve::createApplication()
{
	return new ExampleApp();
}