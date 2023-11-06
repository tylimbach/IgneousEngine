#include "movement_system.h"

#include "components/components.h"

namespace bve
{
	MovementSystem::MovementSystem(EntityManager& entityManager) : entityManager(entityManager)
	{
	}

	void MovementSystem::update(float dt)
	{
		auto movers = entityManager.view<MoveComponent>();
		for (auto&& [entity, moveComp] : movers) {
			if (entityManager.has<TransformComponent>(entity)) {
				auto& transformComp = entityManager.get<TransformComponent>(entity);
                moveComp.velocity += moveComp.acceleration * dt;
				transformComp.translation += moveComp.velocity * dt;
			}
		}

		auto rotators = entityManager.view<RotateComponent>();
		for (auto&& [entity, rotateComp] : rotators) {
            if (entityManager.has<TransformComponent>(entity)) {
                auto& transformComp = entityManager.get<TransformComponent>(entity);
                rotateComp.velocity += rotateComp.acceleration * dt;
                transformComp.rotation += rotateComp.velocity * dt;

                for (int i = 0; i < 3; ++i) {
                    if (transformComp.rotation[i] > 2.0f * glm::pi<float>()) {
                        transformComp.rotation[i] -= 2.0f * glm::pi<float>();
                    }
                    else if (transformComp.rotation[i] < 0) {
                        transformComp.rotation[i] += 2.0f * glm::pi<float>();
                    }
                }
            }
		}
	}
}
