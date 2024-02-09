#pragma once

#include "defines.h"
#include "entity_manager.h"

namespace bve
{
	IG_API class Application
	{
	public:
		static constexpr int WIDTH = 1600;
		static constexpr int HEIGHT = 1200;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(const Application&&) = delete;
		Application& operator=(const Application&&) = delete;

		void run();

	private:
		EntityManager entityManager_{};
	};

	Application* createApplication();

}