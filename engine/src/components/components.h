#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../defines.h"

#include "../bve_model.h"
#include <memory>

namespace bve
{
	struct IG_API TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{1.f, 1.f, 1.f};
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4()
		{
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			return glm::mat4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f,
				},
				{translation.x, translation.y, translation.z, 1.0f}
			};
		}

		glm::mat3 normalMatrix()
		{
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);
			const glm::vec3 invScale = 1.0f / scale;

			return glm::mat3{
				{
					invScale.x * (c1 * c3 + s1 * s2 * s3),
					invScale.x * (c2 * s3),
					invScale.x * (c1 * s2 * s3 - c3 * s1),
				},
				{
					invScale.y * (c3 * s1 * s2 - c1 * s3),
					invScale.y * (c2 * c3),
					invScale.y * (c1 * c3 * s2 + s1 * s3),
				},
				{
					invScale.z * (c2 * s1),
					invScale.z * (-s2),
					invScale.z * (c1 * c2),
				},
			};
		}
	};

	struct IG_API MoveComponent
	{
		glm::vec3 velocity{.0f, .0f, .0f};
		glm::vec3 acceleration{.0f, .0f, .0f};
	};

	struct IG_API RotateComponent
	{
		glm::vec3 velocity{.0f, .0f, .0f};
		glm::vec3 acceleration{.0f, .0f, .0f};
	};

	enum class IG_API ProjectionMode
	{
		PERSPECTIVE,
		ORTHOGRAPHIC,
	};

	struct IG_API CameraComponent
	{
		ProjectionMode mode{ProjectionMode::PERSPECTIVE};

		float fovyDegrees{1}, aspect{16.f / 9.f};
		float left{-1.f}, right{1.f}, top{1.f}, bottom{-1.f};

		float near{0.1f}, far{100.f};
		glm::mat4 projectionMatrix{1.f};
		glm::mat4 viewMatrix{1.f};
		glm::mat4 inverseViewMatrix{1.f};
	};

	struct IG_API RenderComponent
	{
		std::unique_ptr<BveModel> model;
		glm::vec3 color{.0f, .0f, .0f};
	};

	struct IG_API PointLightComponent
	{
		glm::vec4 color{.0f, .0f, .0f, .0f};
	};

	struct IG_API PlayerTag {};

	struct IG_API ActiveCameraTag {};

	struct IG_API SelectedTag {};
}
