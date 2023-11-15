#include <engine.h>

#include <iostream>

class ExampleApp : public bve::Application
{
public:
	ExampleApp() {};
	~ExampleApp() {};
};

bve::Application* bve::CreateApplication()
{
	return new ExampleApp();
}