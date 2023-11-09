#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "entity_manager.h"
#include "components/components.h"

namespace bve
{
	class CameraSystem
	{
	public:
		CameraSystem(EntityManager& entityManager, Entity camera = UINT32_MAX);
		~CameraSystem() = default;

		void update(float aspectRatio);
		Entity getActiveCamera() const { return activeCamera_; }

		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up);
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up);

	private:
		static void setOrthographicProjection(CameraComponent& camera);
		static void setPerspectiveProjection(CameraComponent& camera);
		static void setView(CameraComponent& camera, TransformComponent& transform);
		static void directionAndUpToEulerYXZ(const glm::vec3& direction, const glm::vec3& up, glm::vec3& rotation);

		EntityManager& entityManager_;
		Entity activeCamera_;
	};
}
