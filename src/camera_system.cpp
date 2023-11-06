#include "camera_system.h"

#include "movement_system.h"
#include "components/components.h"

namespace bve
{
	CameraSystem::CameraSystem(EntityManager& entityManager, Entity camera) : entityManager(entityManager)
	{
		if (camera == UINT32_MAX) {
			camera = entityManager.createEntity();
			entityManager.add<TransformComponent, MoveComponent, RotateComponent, ActiveCameraTag, CameraComponent>(camera);
		}

		activeCamera = camera;
	}

	void CameraSystem::update(float aspectRatio)
	{
		auto cameras = entityManager.view<CameraComponent>();
		for (auto&& [entity, cameraComp] : cameras) {
			assert(entityManager.has<TransformComponent>(entity) && "Camera must have a transform component");

			cameraComp.aspect = aspectRatio;

			switch (cameraComp.mode) {
				case CameraMode::PERSPECTIVE:
					setPerspectiveProjection(cameraComp);
					break;
				case CameraMode::ORTHOGRAPHIC:
					setOrthographicProjection(cameraComp);
					break;
			}

			auto& transformComp = entityManager.get<TransformComponent>(entity);
			setView(cameraComp, transformComp);
		}
	}

	void CameraSystem::setOrthographicProjection(CameraComponent& camera) {
		camera.projectionMatrix[0][0] = 2.f / (camera.right - camera.left);
		camera.projectionMatrix[1][1] = 2.f / (camera.bottom - camera.top);
		camera.projectionMatrix[2][2] = 1.f / (camera.far - camera.near);
		camera.projectionMatrix[3][0] = -(camera.right + camera.left) / (camera.right - camera.left);
		camera.projectionMatrix[3][1] = -(camera.bottom + camera.top) / (camera.bottom - camera.top);
		camera.projectionMatrix[3][2] = -camera.near / (camera.far - camera.near);
	}

	void CameraSystem::setPerspectiveProjection(CameraComponent& camera) {
		assert(glm::abs(camera.aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tanHalfFovy = tan(camera.fovy / 2.f);

		camera.projectionMatrix[0][0] = 1.f / (camera.aspect * tanHalfFovy);
		camera.projectionMatrix[1][1] = 1.f / (tanHalfFovy);
		camera.projectionMatrix[2][2] = camera.far / (camera.far - camera.near);
		camera.projectionMatrix[2][3] = 1.f;
		camera.projectionMatrix[3][2] = -(camera.far * camera.near) / (camera.far - camera.near);
	}

	void CameraSystem::setView(CameraComponent& camera, TransformComponent& transform) {
		const float c3 = glm::cos(transform.rotation.z);
		const float s3 = glm::sin(transform.rotation.z);
		const float c2 = glm::cos(transform.rotation.x);
		const float s2 = glm::sin(transform.rotation.x);
		const float c1 = glm::cos(transform.rotation.y);
		const float s1 = glm::sin(transform.rotation.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		camera.viewMatrix[0][0] = u.x;
		camera.viewMatrix[1][0] = u.y;
		camera.viewMatrix[2][0] = u.z;
		camera.viewMatrix[0][1] = v.x;
		camera.viewMatrix[1][1] = v.y;
		camera.viewMatrix[2][1] = v.z;
		camera.viewMatrix[0][2] = w.x;
		camera.viewMatrix[1][2] = w.y;
		camera.viewMatrix[2][2] = w.z;
		camera.viewMatrix[3][0] = -glm::dot(u, transform.translation);
		camera.viewMatrix[3][1] = -glm::dot(v, transform.translation);
		camera.viewMatrix[3][2] = -glm::dot(w, transform.translation);
	}
}
