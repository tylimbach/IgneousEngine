#include "basic_app.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <array>
#include <thread>

namespace bve {

	BasicApp::BasicApp() {
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	BasicApp::~BasicApp() {
		vkDestroyPipelineLayout(bveDevice.device(), pipelineLayout, nullptr);
	}

	void BasicApp::run() {
		while (!bveWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(bveDevice.device());
	}

	void BasicApp::generateVertices(int subdivisionIterations, std::vector<BveModel::Vertex> &inVertices, std::vector<BveModel::Vertex> &outVertices) {
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

		glm::vec2 position = { (inVertices.back().position + inVertices[0].position) * 0.5f};
		glm::vec3 color = { (inVertices.back().color + inVertices[0].color) * 0.5f };
		newVertices.push_back({ position, color });

		for (int i = 0; i < inVertices.size() - 1; i++) {
			newShapes.push_back({ newVertices[i], inVertices[i + 1], newVertices[i + 1]});
			generateVertices(subdivisionIterations - 1, newShapes[i], outVertices);
		}
		newShapes.push_back({ newVertices.back(), inVertices[0], newVertices[0]});
		generateVertices(subdivisionIterations - 1, newShapes.back(), outVertices);
	}

	void BasicApp::loadModels() {
		std::vector<BveModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		std::vector<BveModel::Vertex> complexVertices{};
		generateVertices(8, vertices, complexVertices);
		
		bveModel = std::make_unique<BveModel>(bveDevice, complexVertices);
	}

	void BasicApp::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
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
		clearValues[0].color = {{0.1f, 0.1f, 0.1f, 1.0f}};
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

		bvePipeline->bind(commandBuffers[imageIndex]);
		bveModel->bind(commandBuffers[imageIndex]);
		bveModel->draw(commandBuffers[imageIndex]);

		vkCmdDraw(commandBuffers[imageIndex], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);

		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
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