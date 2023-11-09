#include "input_controller.h"
#include "components/components.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "glm/glm.hpp"

#include <limits>

namespace bve
{
	InputController::InputController(EntityManager& entityManager) : entityManager_(entityManager) {}
	InputController::~InputController() {}

	void InputController::update(GLFWwindow* window, Entity entity)
	{
		auto&& transformComp = entityManager_.get<TransformComponent>(entity);
		auto&& moveComp = entityManager_.get<MoveComponent>(entity);
		auto&& rotateComp = entityManager_.get<RotateComponent>(entity);

		glm::vec3 rotate{0};
		if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		// Update RotateComponent's velocity
		if (dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
			rotateComp.velocity = lookSpeed * normalize(rotate);
		} else {
			rotateComp.velocity = {0.f, 0.f, 0.f};
		}

		float yaw = transformComp.rotation.y;
		const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
		const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
		constexpr glm::vec3 upDir{0.f, -1.f, 0.f};

		glm::vec3 moveDir{0.f};
		if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		// Update MoveComponent's velocity
		if (dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
			moveComp.velocity = moveSpeed * normalize(moveDir);
		} else {
			moveComp.velocity = {0.f, 0.f, 0.f};
		}
	}
}
