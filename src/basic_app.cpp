#include "basic_app.h"

#include "input_controller.h"
#include "entity_manager.h"
#include "components/components.h"
#include "systems/render_system.h"
#include "systems/camera_system.h"
#include "systems/point_light_render_system.h"
#include "systems/movement_system.h"
#include "master_renderer.h"
#include "vulkan_buffer.h"

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

namespace bve
{
	BasicApp::BasicApp()
	{
		loadEntities();
	}

	BasicApp::~BasicApp() = default;

	void BasicApp::run()
	{
		//BveImgui gui{bveWindow_, bveDevice_, renderer_.getSwapChainRenderPass(), renderer_.getImageCount(), entityManager_};
		//RenderSystem renderSystem{bveDevice_, renderer_.getSwapChainRenderPass(), entityManager_};
		//PointLightRenderSystem pointLightRenderSystem{bveDevice_, renderer_.getSwapChainRenderPass(), entityManager_};

		InputController inputController{entityManager_};

		MasterRenderer renderer{bveWindow_, bveDevice_, entityManager_};

		CameraSystem cameraSystem{entityManager_};
		MovementSystem movementSystem{entityManager_};

		float aspectRatio = renderer.getAspectRatio();
		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!bveWindow_.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			const float frameDt = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			inputController.update(bveWindow_.getGLFWWindow());
			movementSystem.update(frameDt);
			cameraSystem.update(aspectRatio);

			if (!renderer.renderFrame(frameDt)) {
				aspectRatio = renderer.getAspectRatio();
			}
		}

		vkDeviceWaitIdle(bveDevice_.device());
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
			glm::vec3 position = {(inVertices[i].position + inVertices[i + 1].position) * 0.5f};
			glm::vec3 color = {(inVertices[i].color + inVertices[i + 1].color) * 0.5f};
			newVertices.push_back({position, color});
		}

		glm::vec3 position = {(inVertices.back().position + inVertices[0].position) * 0.5f};
		glm::vec3 color = {(inVertices.back().color + inVertices[0].color) * 0.5f};
		newVertices.push_back({position, color});

		for (int i = 0; i < inVertices.size() - 1; i++) {
			newShapes.push_back({newVertices[i], inVertices[i + 1], newVertices[i + 1]});
			subdivideTriangle(subdivisionIterations - 1, newShapes[i], outVertices);
		}
		newShapes.push_back({newVertices.back(), inVertices[0], newVertices[0]});
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
		const Entity modelEntity = entityManager_.createEntity();
		std::unique_ptr<BveModel> model = BveModel::createModelFromFile(bveDevice_, "models/LowPolyCharacter.obj");
		entityManager_.addComponent<RenderComponent>(modelEntity, {std::move(model), glm::vec3{}});
		entityManager_.addComponent<TransformComponent>(modelEntity, {{.0f, .5f, 0.0f}});
		entityManager_.addComponent<MoveComponent, RotateComponent, PlayerTag>(modelEntity);

		const Entity cubeEntity = entityManager_.createEntity();
		std::unique_ptr<BveModel> cubeModel = BveModel::createModelFromFile(bveDevice_, "models/colored_cube.obj");
		entityManager_.addComponent<RenderComponent>(cubeEntity, {std::move(cubeModel), glm::vec3{}});
		entityManager_.addComponent<TransformComponent>(cubeEntity, {{-2.5f, -2.5f, -0.5f}});
		entityManager_.addComponent<MoveComponent, RotateComponent, PlayerTag>(cubeEntity);

		const Entity floorEntity = entityManager_.createEntity();
		std::unique_ptr<BveModel> floorModel = BveModel::createModelFromFile(bveDevice_, "models/quad.obj");
		entityManager_.addComponent<RenderComponent>(floorEntity, {std::move(floorModel), glm::vec3{}});
		entityManager_.addComponent<TransformComponent>(floorEntity, {{0.5f, 0.5f, 0.f}, {25.f, 1.f, 25.f}});
		entityManager_.addComponent<MoveComponent, RotateComponent, PlayerTag>(floorEntity);

		const Entity lightEntity = entityManager_.createEntity();
		entityManager_.addComponent<TransformComponent, MoveComponent>(lightEntity);
		entityManager_.addComponent<PointLightComponent>(lightEntity, {{1.0f, 0.2f, 0.2f, 5.f}});

		const Entity lightEntity2 = entityManager_.createEntity();
		entityManager_.addComponent<TransformComponent, MoveComponent>(lightEntity2);
		entityManager_.addComponent<PointLightComponent>(lightEntity2, {{1.f, 1.0f, 1.f, 2.5f}});

		const Entity lightEntity3 = entityManager_.createEntity();
		entityManager_.addComponent<TransformComponent, MoveComponent>(lightEntity3);
		entityManager_.addComponent<PointLightComponent>(lightEntity3, {{0.2f, 0.2f, 1.0f, 10.f}});
	}
}
