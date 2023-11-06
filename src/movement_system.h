#pragma once

#include "entity_manager.h"

namespace bve 
{
	class MovementSystem
	{
	public:
		MovementSystem(EntityManager& entityManager);
		~MovementSystem() = default;

		void update(float dt);

	private:
		EntityManager& entityManager;
	};
}