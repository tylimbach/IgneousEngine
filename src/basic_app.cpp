#include "basic_app.h"

#include "input_controller.h"
#include "entity_manager.h"
#include "components/components.h"
#include "render_system.h"
#include "camera_system.h"
#include "movement_system.h"

#include "bve_imgui.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>

namespace bve {

	BasicApp::BasicApp() {
		loadEntities();
	}

	BasicApp::~BasicApp() = default;

	void BasicApp::run()
	{
		BveImgui bveImGui{ bveWindow, bveDevice, renderer.getSwapChainRenderPass(), renderer.getImageCount() };

		InputController inputController{ entityManager };

		RenderSystem renderSystem{ bveDevice, renderer.getSwapChainRenderPass(), entityManager };
		CameraSystem cameraSystem{ entityManager };
		MovementSystem movementSystem{ entityManager };

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!bveWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			inputController.update(bveWindow.getGLFWWindow(), 0);

			const float aspectRatio = renderer.getAspectRatio();

			movementSystem.update(frameTime);
			cameraSystem.update(aspectRatio);

			Entity camera = cameraSystem.getActiveCamera();

			if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
				bveImGui.newFrame();
				
				renderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.render(commandBuffer, camera);

				bveImGui.runExample();
				bveImGui.render(commandBuffer);

				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(bveDevice.device());
	}

	std::unique_ptr<BveModel> createCubeModel(BveDevice& device, glm::vec3 offset) {
		BveModel::Builder modelBuilder{};
		modelBuilder.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};
		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<BveModel>(device, modelBuilder);
	}

	void subdivideTriangle(int subdivisionIterations, std::vector<BveModel::Vertex>& inVertices, std::vector<BveModel::Vertex>& outVertices)
	{
		if (subdivisionIterations == 0) {
			outVertices.insert(std::end(outVertices), std::begin(inVertices), std::end(inVertices));
			return;
		}

		std::vector<BveModel::Vertex> newVertices{};
		std::vector<std::vector<BveModel::Vertex>> newShapes{};
		for (int i = 0; i < inVertices.size() - 1; i++) {
			glm::vec3 position = { (inVertices[i].position + inVertices[i + 1].position) * 0.5f };
			glm::vec3 color = { (inVertices[i].color + inVertices[i + 1].color) * 0.5f };
			newVertices.push_back({ position, color });
		}

		glm::vec3 position = { (inVertices.back().position + inVertices[0].position) * 0.5f };
		glm::vec3 color = { (inVertices.back().color + inVertices[0].color) * 0.5f };
		newVertices.push_back({ position, color });

		for (int i = 0; i < inVertices.size() - 1; i++) {
			newShapes.push_back({ newVertices[i], inVertices[i + 1], newVertices[i + 1] });
			subdivideTriangle(subdivisionIterations - 1, newShapes[i], outVertices);
		}
		newShapes.push_back({ newVertices.back(), inVertices[0], newVertices[0] });
		subdivideTriangle(subdivisionIterations - 1, newShapes.back(), outVertices);
	}

	std::unique_ptr<BveModel> createTriangleModel(BveDevice& device, glm::vec3 offset, int subdivisions)
	{
		BveModel::Builder modelBuilder;

		std::vector<BveModel::Vertex> basicVertices{
			{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
		};

		subdivideTriangle(subdivisions, basicVertices, modelBuilder.vertices);

		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

		return std::make_unique<BveModel>(device, modelBuilder);
	}

	void BasicApp::loadEntities()
	{
		const Entity cube = entityManager.createEntity();
		std::unique_ptr<BveModel> cubeModel = createCubeModel(bveDevice, {0.f, 0.f, 0.f});

		entityManager.add<RenderComponent>(cube, { std::move(cubeModel), glm::vec3{} });
		entityManager.add<TransformComponent>(cube, { { .0f, .0f, 25.f }, {.5f, .5f, .5f} });
		entityManager.add<MoveComponent, RotateComponent, PlayerTag>(cube);

		//std::shared_ptr<BveModel> triangleModel = createTriangleModel(bveDevice, { 0.f, 0.f, 0.f }, 3);
		//Entity triangle = entityManager.createEntity();
		//RenderComponent triangleRenderCmp{ triangleModel, glm::vec3{} };
		//TransformComponent triangleTransformCmp{ {.0f, .0f, .0f}, {.5f, .5f, .5f}};
		//entityManager.add<RenderComponent>(triangle, triangleRenderCmp);
		//entityManager.add<TransformComponent>(triangle, triangleTransformCmp);

	}
}
