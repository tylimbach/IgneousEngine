#include "camera_system.h"

#include "movement_system.h"
#include "../components/components.h"
#include <glm/gtc/matrix_transform.hpp>

namespace bve
{
	CameraSystem::CameraSystem(EntityManager& entityManager, Entity camera) : entityManager_(entityManager)
	{
		if (camera == UINT32_MAX) {
			camera = entityManager.createEntity("Default Camera");
			entityManager.addComponent<MoveComponent, RotateComponent, ActiveCameraTag, CameraComponent>(camera);
			entityManager.addComponent<TransformComponent>(camera, TransformComponent{{0.f, -2.5f, -5.f}, {}, {}});
		}

		activeCamera_ = camera;
	}

	void CameraSystem::update(float aspectRatio)
	{
		auto cameras = entityManager_.view<CameraComponent>();
		for (auto&& [entity, cameraComp] : cameras) {
			assert(entityManager_.hasComponent<TransformComponent>(entity) && "Camera must have a transform component");

			cameraComp.aspect = aspectRatio;

			switch (cameraComp.mode) {
			case ProjectionMode::PERSPECTIVE:
				setPerspectiveProjection(cameraComp);
				break;
			case ProjectionMode::ORTHOGRAPHIC:
				setOrthographicProjection(cameraComp);
				break;
			}

			auto& transformComp = entityManager_.getComponent<TransformComponent>(entity);
			setView(cameraComp, transformComp);
		}
	}

	void CameraSystem::setOrthographicProjection(CameraComponent& camera)
	{
		camera.projectionMatrix[0][0] = 2.f / (camera.right - camera.left);
		camera.projectionMatrix[1][1] = 2.f / (camera.bottom - camera.top);
		camera.projectionMatrix[2][2] = 1.f / (camera.far - camera.near);
		camera.projectionMatrix[3][0] = -(camera.right + camera.left) / (camera.right - camera.left);
		camera.projectionMatrix[3][1] = -(camera.bottom + camera.top) / (camera.bottom - camera.top);
		camera.projectionMatrix[3][2] = -camera.near / (camera.far - camera.near);
	}

	void CameraSystem::setPerspectiveProjection(CameraComponent& camera)
	{
		assert(glm::abs(camera.aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tanHalfFovy = tan(camera.fovyDegrees / 2.f);

		camera.projectionMatrix[0][0] = 1.f / (camera.aspect * tanHalfFovy);
		camera.projectionMatrix[1][1] = 1.f / (tanHalfFovy);
		camera.projectionMatrix[2][2] = camera.far / (camera.far - camera.near);
		camera.projectionMatrix[2][3] = 1.f;
		camera.projectionMatrix[3][2] = -(camera.far * camera.near) / (camera.far - camera.near);
	}

	void CameraSystem::setView(CameraComponent& camera, TransformComponent& transform)
	{
		const float c3 = glm::cos(transform.rotation.z);
		const float s3 = glm::sin(transform.rotation.z);
		const float c2 = glm::cos(transform.rotation.x);
		const float s2 = glm::sin(transform.rotation.x);
		const float c1 = glm::cos(transform.rotation.y);
		const float s1 = glm::sin(transform.rotation.y);
		const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
		const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
		const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
		camera.viewMatrix[0][0] = u.x;
		camera.viewMatrix[1][0] = u.y;
		camera.viewMatrix[2][0] = u.z;
		camera.viewMatrix[0][1] = v.x;
		camera.viewMatrix[1][1] = v.y;
		camera.viewMatrix[2][1] = v.z;
		camera.viewMatrix[0][2] = w.x;
		camera.viewMatrix[1][2] = w.y;
		camera.viewMatrix[2][2] = w.z;
		camera.viewMatrix[3][0] = -dot(u, transform.translation);
		camera.viewMatrix[3][1] = -dot(v, transform.translation);
		camera.viewMatrix[3][2] = -dot(w, transform.translation);

		camera.inverseViewMatrix[0][0] = u.x;
		camera.inverseViewMatrix[0][1] = u.y;
		camera.inverseViewMatrix[0][2] = u.z;
		camera.inverseViewMatrix[1][0] = v.x;
		camera.inverseViewMatrix[1][1] = v.y;
		camera.inverseViewMatrix[1][2] = v.z;
		camera.inverseViewMatrix[2][0] = w.x;
		camera.inverseViewMatrix[2][1] = w.y;
		camera.inverseViewMatrix[2][2] = w.z;
		camera.inverseViewMatrix[3][0] = transform.translation.x;
		camera.inverseViewMatrix[3][1] = transform.translation.y;
		camera.inverseViewMatrix[3][2] = transform.translation.z;
	}

	void CameraSystem::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up)
	{
		auto&& transformComp = entityManager_.getComponent<TransformComponent>(activeCamera_);

		transformComp.translation = position;
		directionAndUpToEulerYXZ(direction, up, transformComp.rotation);
	}

	void CameraSystem::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up)
	{
		setViewDirection(position, target - position, up);
	}

	void CameraSystem::directionAndUpToEulerYXZ(const glm::vec3& direction, const glm::vec3& up, glm::vec3& rotation)
	{
		const glm::vec3 forward = normalize(direction);

		rotation.y = glm::atan(forward.x, forward.z); // yaw
		rotation.x = glm::atan(-forward.y, glm::sqrt(forward.x * forward.x + forward.z * forward.z)); // pitch

		// Now we can calculate the roll by comparing the original up vector with the up vector generated by yaw and pitch
		// This is a simplification and might not work correctly if pitch is near +-90 degrees (gimbal lock situation)
		const glm::vec3 right = normalize(cross(up, forward));
		const glm::vec3 newUp = cross(forward, right);
		rotation.z = glm::atan(dot(newUp, up), dot(cross(right, newUp), up));
	}
}