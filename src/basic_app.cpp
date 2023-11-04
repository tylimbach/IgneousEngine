#include "basic_app.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <array>
#include <thread>

#include "entity_manager.h"
#include "components/components.h"

namespace bve {

	struct SimplePushConstantData {
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	BasicApp::BasicApp() {
		loadEntities();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	BasicApp::~BasicApp() {
		vkDestroyPipelineLayout(bveDevice.device(), pipelineLayout, nullptr);
	}

	void BasicApp::run()
	{
		while (!bveWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(bveDevice.device());
	}

	void BasicApp::generateVertices(int subdivisionIterations, std::vector<BveModel::Vertex>& inVertices, std::vector<BveModel::Vertex>& outVertices)
	{
		if (subdivisionIterations == 0) {
			outVertices.insert(std::end(outVertices), std::begin(inVertices), std::end(inVertices));
			return;
		}

		std::vector<BveModel::Vertex> newVertices{};
		std::vector<std::vector<BveModel::Vertex>> newShapes{};
		for (int i = 0; i < inVertices.size() - 1; i++) {
			glm::vec2 position = { (inVertices[i].position + inVertices[i + 1].position) * 0.5f };
			glm::vec3 color = { (inVertices[i].color + inVertices[i + 1].color) * 0.5f };
			newVertices.push_back({ position, color });
		}

		glm::vec2 position = { (inVertices.back().position + inVertices[0].position) * 0.5f };
		glm::vec3 color = { (inVertices.back().color + inVertices[0].color) * 0.5f };
		newVertices.push_back({ position, color });

		for (int i = 0; i < inVertices.size() - 1; i++) {
			newShapes.push_back({ newVertices[i], inVertices[i + 1], newVertices[i + 1] });
			generateVertices(subdivisionIterations - 1, newShapes[i], outVertices);
		}
		newShapes.push_back({ newVertices.back(), inVertices[0], newVertices[0] });
		generateVertices(subdivisionIterations - 1, newShapes.back(), outVertices);
	}

	void BasicApp::loadEntities()
	{
		Entity firstTriangle = entityManager.createEntity();
		std::vector<BveModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};
		std::vector<BveModel::Vertex> complexVertices{};
		generateVertices(6, vertices, complexVertices);

		RenderComponent renderCmp1(make_shared<BveModel>(bveDevice, complexVertices), { .1f, .8f, .1f });
		entityManager.addComponent<RenderComponent>(firstTriangle, renderCmp1);

		Transform2dComponent transformCmp1{};
		entityManager.addComponent<Transform2dComponent>(firstTriangle, transformCmp1);

		Entity secondTriangle = entityManager.createEntity();
		std::vector<BveModel::Vertex> vertices2{
			{{0.5f, 0.5f}, {0.6f, 1.0f, 0.0f}},
			{{0.0f, 0.85f}, {0.0f, 0.6f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 0.0f, 0.6f}}
		};
		std::vector<BveModel::Vertex> complexVertices2{};
		generateVertices(3, vertices2, complexVertices2);

		RenderComponent renderCmp2(make_shared<BveModel>(bveDevice, complexVertices2), { .8f, .1f, .1f });
		entityManager.addComponent<RenderComponent>(secondTriangle, renderCmp2);

		Transform2dComponent transformCmp2{};
		entityManager.addComponent<Transform2dComponent>(secondTriangle, transformCmp2);
	}

	void BasicApp::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(bveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline info");
		}
	}

	void BasicApp::createPipeline()
	{
		assert(bveSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		BvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = bveSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		bvePipeline = std::make_unique<BvePipeline>(
			bveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void BasicApp::recreateSwapChain()
	{
		VkExtent2D extent = bveWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = bveWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(bveDevice.device());

		if (bveSwapChain == nullptr) {
			bveSwapChain = std::make_unique<BveSwapChain>(bveDevice, extent);
		} else {
			bveSwapChain = std::make_unique<BveSwapChain>(bveDevice, extent, std::move(bveSwapChain));
			if (bveSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

		bveSwapChain = nullptr;
		bveSwapChain = std::make_unique<BveSwapChain>(bveDevice, extent);
		createPipeline();
	}

	void BasicApp::createCommandBuffers()
	{
		commandBuffers.resize(bveSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = bveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(bveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void BasicApp::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			bveDevice.device(),
			bveDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	void BasicApp::recordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = bveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = bveSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = bveSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.01f, 0.01f, 0.01f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(bveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(bveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, bveSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		render(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);

		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}
	}

	void BasicApp::render(VkCommandBuffer commandBuffer)
	{
		bvePipeline->bind(commandBuffer);

		EntityComponentView<RenderComponent> view = entityManager.view<RenderComponent>();
		for (const auto&& [entity, modelComponent] : view) {
			SimplePushConstantData push{};

			push.color = modelComponent.color;
			if (entityManager.hasComponent<Transform2dComponent>(entity)) {
				Transform2dComponent& transformComponent = entityManager.getComponent<Transform2dComponent>(entity);
				push.offset = transformComponent.translation;
				push.transform = transformComponent.mat2();

				transformComponent.rotation = glm::mod(transformComponent.rotation + 0.001f, glm::two_pi<float>());
			}

			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			modelComponent.model->bind(commandBuffer);
			modelComponent.model->draw(commandBuffer);
		}
	}

	void BasicApp::drawFrame()
	{
		uint32_t imageIndex;
		VkResult result = bveSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		recordCommandBuffer(imageIndex);
		result = bveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || bveWindow.wasWindowResized()) {
			bveWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}
	}
}