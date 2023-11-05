#include "basic_app.h"

#include "camera.h"
#include "entity_manager.h"
#include "components/components.h"
#include "render_system.h"

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

namespace bve {

	BasicApp::BasicApp() {
		loadEntities();
	}

	BasicApp::~BasicApp() = default;

	void BasicApp::run()
	{
		BveImgui bveImgui{ bveWindow, bveDevice, renderer.getSwapChainRenderPass(), renderer.getImageCount() };

		RenderSystem renderSystem{ bveDevice, renderer.getSwapChainRenderPass(), entityManager };
		Camera camera{};
		camera.setViewDirection(glm::vec3{ 0.f }, glm::vec3{ 0.5f, 0.f, 1.f });

		while (!bveWindow.shouldClose()) {
			glfwPollEvents();

			float aspect = renderer.getAspectRatio();
			//camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

			if (VkCommandBuffer commandBuffer = renderer.beginFrame()) {
				bveImgui.newFrame();

				// render game objects first, so they will be rendered in the background. This
				// is the best we can do for now.
				// Once we cover offscreen rendering, we can render the scene to a image/texture rather than
				// directly to the swap chain. This texture of the scene can then be rendered to an imgui
				// subwindow
				renderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.render(commandBuffer, camera);

				// example code telling imgui what windows to render, and their contents
				// this can be replaced with whatever code/classes you set up configuring your
				// desired engine UI
				bveImgui.runExample();

				// as last step in render pass, record the imgui draw commands
				bveImgui.render(commandBuffer);

				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(bveDevice.device());
	}

	std::unique_ptr<BveModel> createCubeModel(BveDevice& device, glm::vec3 offset) {
		std::vector<BveModel::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<BveModel>(device, vertices);
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

	std::unique_ptr<BveModel> createTriangleModel(BveDevice& device, glm::vec3 offset, int subdivisions) {
		std::vector<BveModel::Vertex> basicVertices{
			{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
		};

		std::vector<BveModel::Vertex> modifiedVertices{};
		subdivideTriangle(subdivisions, basicVertices, modifiedVertices);

		for (auto& v : modifiedVertices) {
			v.position += offset;
		}
		return std::make_unique<BveModel>(device, modifiedVertices);
	}

	void BasicApp::loadEntities()
	{
		std::unique_ptr<BveModel> cubeModel = createCubeModel(bveDevice, {0.f, 0.f, 0.f});
		//std::shared_ptr<BveModel> triangleModel = createTriangleModel(bveDevice, { 0.f, 0.f, 0.f }, 3);

		Entity cube = entityManager.createEntity();
		//Entity triangle = entityManager.createEntity();

		RenderComponent cubeRenderCmp{ std::move(cubeModel), glm::vec3{} };
		//RenderComponent triangleRenderCmp{ triangleModel, glm::vec3{} };

		TransformComponent cubeTransformCmp{ { .0f, .0f, 2.5f }, {.5f, .5f, .5f}};
		//TransformComponent triangleTransformCmp{ {.0f, .0f, .0f}, {.5f, .5f, .5f}};

		entityManager.addComponent<RenderComponent>(cube, std::move(cubeRenderCmp));
		//entityManager.addComponent<RenderComponent>(triangle, triangleRenderCmp);
		entityManager.addComponent<TransformComponent>(cube, cubeTransformCmp);
		//entityManager.addComponent<TransformComponent>(triangle, triangleTransformCmp);
	}
}