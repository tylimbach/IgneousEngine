#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "../bve_model.h"
#include <memory>

namespace bve {
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4() {
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
                {translation.x, translation.y, translation.z, 1.0f} };
        }
	};

    struct MoveComponent
    {
        glm::vec3 velocity{ .0f, .0f, .0f };
        glm::vec3 acceleration{ .0f, .0f, .0f };
    };

    struct RotateComponent
    {
        glm::vec3 velocity{ .0f, .0f, .0f };
        glm::vec3 acceleration{ .0f, .0f, .0f };
    };

    enum class CameraMode {
        PERSPECTIVE,
        ORTHOGRAPHIC,
    };

    struct CameraComponent
    {
        CameraMode mode{CameraMode::PERSPECTIVE};

        float fovy{ 50.f }, aspect{ 16.f / 9.f };
        float left{ -1.f }, right{ 1.f }, top{ 1.f }, bottom{-1.f};

        float near{ 0.1f }, far{1000.f};
    	glm::mat4 projectionMatrix{ 1.f };
        glm::mat4 viewMatrix{ 1.f };
    };

	struct RenderComponent
	{
		std::unique_ptr<BveModel> model;
		glm::vec3 color{ .0f, .0f, .0f};
	};

    struct PlayerTag{};
    struct ActiveCameraTag{};
}
