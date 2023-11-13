#pragma once

#include "../entity_manager.h"

namespace bve
{
	class MovementSystem
	{
	public:
		MovementSystem(EntityManager& entityManager);

		void update(float dt);

	private:
		EntityManager& entityManager_;
	};
}
