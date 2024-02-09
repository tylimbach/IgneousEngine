#include "pch.h"
#include "application.h"
#include "input_controller.h"
#include "entity_manager.h"
#include "components/components.h"
#include "systems/render_system.h"
#include "systems/camera_system.h"
#include "systems/point_light_render_system.h"
#include "systems/movement_system.h"
#include "master_renderer.h"
#include "log.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>

namespace bve
{
	Application::Application()
	{
	}

	Application::~Application() = default;

	void loadEntities(EntityManager& entityManager, BveDevice& device)
	{
		LOG_INFO("loading entities");
		const Entity modelEntity = entityManager.createEntity("Guy");
		std::unique_ptr<BveModel> model = BveModel::createModelFromFile(device, "models/LowPolyCharacter.obj");
		entityManager.addComponent<RenderComponent>(modelEntity, { std::move(model), glm::vec3{} });
		entityManager.addComponent<TransformComponent>(modelEntity, TransformComponent{ {1.f, -1.f, 0.0f} });
		entityManager.addComponent<MoveComponent, RotateComponent>(modelEntity);

		//const Entity cubeEntity = entityManager.createEntity("Cube");
		//std::unique_ptr<BveModel> cubeModel = BveModel::createModelFromFile(bveDevice_, "models/colored_cube.obj");
		//entityManager.addComponent<RenderComponent>(cubeEntity, {std::move(cubeModel), glm::vec3{}});
		//entityManager.addComponent<TransformComponent>(cubeEntity, {{-2.5f, -2.5f, -0.5f}});
		//entityManager.addComponent<MoveComponent, RotateComponent, PlayerTag>(cubeEntity);

		const Entity floorEntity = entityManager.createEntity("Floor");
		std::unique_ptr<BveModel> floorModel = BveModel::createModelFromFile(device, "models/quad.obj");
		entityManager.addComponent<RenderComponent>(floorEntity, { std::move(floorModel), glm::vec3{} });
		entityManager.addComponent<TransformComponent>(floorEntity, { {0.5f, 0.5f, 0.f}, {10.f, 1.f, 10.f} });
		entityManager.addComponent<MoveComponent, RotateComponent>(floorEntity);

		const Entity smoothVase = entityManager.createEntity("Smooth Vase");
		std::unique_ptr<BveModel> smoothVaseModel = BveModel::createModelFromFile(device, "models/smooth_vase.obj");
		entityManager.addComponent<RenderComponent>(smoothVase, { std::move(smoothVaseModel), glm::vec3{} });
		entityManager.addComponent<TransformComponent>(smoothVase, { {0.5f, -0.f, 0.f} });
		entityManager.addComponent<MoveComponent, RotateComponent>(smoothVase);

		const Entity lightEntity = entityManager.createEntity("Red Light");
		entityManager.addComponent<MoveComponent>(lightEntity);
		entityManager.addComponent<TransformComponent>(lightEntity, TransformComponent{ {1.f, -2.f, -1.f}, {0.2f, 0.2f, 0.2f} });
		entityManager.addComponent<PointLightComponent>(lightEntity, { {1.0f, 0.2f, 0.2f, 2.f} });

		const Entity lightEntity2 = entityManager.createEntity("Green Light");
		entityManager.addComponent<MoveComponent>(lightEntity2);
		entityManager.addComponent<TransformComponent>(lightEntity2, TransformComponent{ {0.f, -2.f, 1.f}, {0.2f, 0.2f, 0.2f} });
		entityManager.addComponent<PointLightComponent>(lightEntity2, { {0.2f, 1.0f, 0.2f, 2.f} });

		const Entity lightEntity3 = entityManager.createEntity("Blue Light");
		entityManager.addComponent<MoveComponent>(lightEntity3);
		entityManager.addComponent<TransformComponent>(lightEntity3, TransformComponent{ {-1.f, -2.f, -1.f}, {0.2f, 0.2f, 0.2f} });
		entityManager.addComponent<PointLightComponent>(lightEntity3, { {0.2f, 0.2f, 1.0f, 2.f} });
	}

	void Application::run()
	{
		BveWindow bveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		BveDevice bveDevice{ bveWindow };
		loadEntities(entityManager_, bveDevice);

		InputController inputController{entityManager_};
		MasterRenderer renderer{bveWindow, bveDevice, entityManager_};
		CameraSystem cameraSystem{entityManager_};
		MovementSystem movementSystem{entityManager_};

		float aspectRatio = renderer.getAspectRatio();
		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!bveWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			const float frameDt = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			// process inputs
			inputController.update(bveWindow.getGLFWWindow());

			// physics
			movementSystem.update(frameDt);

			// cameras
			cameraSystem.update(aspectRatio);

			// render

			if (!renderer.renderFrame(frameDt)) {
				aspectRatio = renderer.getAspectRatio();
			}
		}

		vkDeviceWaitIdle(bveDevice.device());
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
}
