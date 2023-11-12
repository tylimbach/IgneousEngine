#pragma once

#include "bve_window.h"
#include "vulkan_renderer.h"
#include "bve_model.h"

#include "entity_manager.h"

namespace bve
{
	class BasicApp
	{
	public:
		static constexpr int WIDTH = 1600;
		static constexpr int HEIGHT = 1200;

		BasicApp();
		~BasicApp();

		BasicApp(const BasicApp&) = delete;
		BasicApp& operator=(const BasicApp&) = delete;
		BasicApp(const BasicApp&&) = delete;
		BasicApp& operator=(const BasicApp&&) = delete;

		void run();

	private:
		void loadEntities();

		BveWindow bveWindow_{WIDTH, HEIGHT, "Hello Vulkan!"};
		BveDevice bveDevice_{bveWindow_};

		EntityManager entityManager_{};
	};
}
