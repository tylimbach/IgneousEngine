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
		Entity getActiveCamera() const { return activeCamera; }

		static void setOrthographicProjection(CameraComponent& camera);
		static void setPerspectiveProjection(CameraComponent& camera);
		static void setView(CameraComponent& camera, TransformComponent& transform);

	private:
		EntityManager& entityManager;
		Entity activeCamera;
	};
}
