#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "entity_manager.h"

namespace bve {
    class InputController {
    public:
        InputController(EntityManager& entityManager);
        ~InputController();

        struct KeyMappings {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_E;
            int moveDown = GLFW_KEY_Q;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void update(GLFWwindow* window, Entity entity);

        KeyMappings keys{};
        float moveSpeed{ 3.f };
        float lookSpeed{ 1.5f };

    private:
        EntityManager& entityManager;
    };
} 