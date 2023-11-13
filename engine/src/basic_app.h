#pragma once

#include "defines.h"
#include "entity_manager.h"

namespace bve
{
	KAPI class BasicApp
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
		EntityManager entityManager_{};
	};
}
