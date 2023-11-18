#include "../pch.h"
#include "movement_system.h"

#include "../components/components.h"

namespace bve
{
	MovementSystem::MovementSystem(EntityManager& entityManager) : entityManager_(entityManager) { }

	void MovementSystem::update(float dt)
	{
		auto movers = entityManager_.view<MoveComponent>();
		for (auto&& [entity, moveComp] : movers) {
			if (entityManager_.hasComponent<TransformComponent>(entity)) {
				auto& transformComp = entityManager_.getComponent<TransformComponent>(entity);
				moveComp.velocity += moveComp.acceleration * dt;
				transformComp.translation += moveComp.velocity * dt;
			}
		}

		auto rotators = entityManager_.view<RotateComponent>();
		for (auto&& [entity, rotateComp] : rotators) {
			if (entityManager_.hasComponent<TransformComponent>(entity)) {
				auto& transformComp = entityManager_.getComponent<TransformComponent>(entity);
				rotateComp.velocity += rotateComp.acceleration * dt;
				transformComp.rotation += rotateComp.velocity * dt;

				for (int i = 0; i < 3; ++i) {
					if (transformComp.rotation[i] > 2.0f * glm::pi<float>()) {
						transformComp.rotation[i] -= 2.0f * glm::pi<float>();
					} else if (transformComp.rotation[i] < 0) {
						transformComp.rotation[i] += 2.0f * glm::pi<float>();
					}
				}
			}
		}
	}
}
