#include "vulkan_renderer.h"

#include <stdexcept>
#include <array>
#include <thread>

#include "entity_manager.h"
#include "components/components.h"

namespace bve
{
	VulkanRenderer::VulkanRenderer(BveWindow& window, BveDevice& device) : bveWindow_(window), bveDevice_(device), currentFrameIndex_(0), isFrameStarted_(false)
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		freeCommandBuffers();
	}

	void VulkanRenderer::recreateSwapChain()
	{
		VkExtent2D extent = bveWindow_.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = bveWindow_.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(bveDevice_.device());

		if (bveSwapChain_ == nullptr) {
			bveSwapChain_ = std::make_unique<BveSwapChain>(bveDevice_, extent);
		} else {
			std::shared_ptr<BveSwapChain> oldSwapChain = std::move(bveSwapChain_);
			bveSwapChain_ = nullptr;
			bveSwapChain_ = std::make_unique<BveSwapChain>(bveDevice_, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*bveSwapChain_)) {
				throw std::runtime_error("Swap chain image or depth format has changed!");
			}
		}
	}

	void VulkanRenderer::createCommandBuffers()
	{
		commandBuffers_.resize(BveSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = bveDevice_.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers_.size());

		if (vkAllocateCommandBuffers(bveDevice_.device(), &allocInfo, commandBuffers_.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void VulkanRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			bveDevice_.device(),
			bveDevice_.getCommandPool(),
			commandBuffers_.size(),
			commandBuffers_.data());
		commandBuffers_.clear();
	}

	VkCommandBuffer VulkanRenderer::beginFrame()
	{
		assert(!isFrameStarted_ && "Cannot call beginFrame while frame already in progress");

		VkResult result = bveSwapChain_->acquireNextImage(&currentImageIndex_);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		isFrameStarted_ = true;

		VkCommandBuffer commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void VulkanRenderer::endFrame()
	{
		assert(isFrameStarted_ && "Cannot call endFrame while frame is not in progress");

		VkCommandBuffer commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}

		VkResult result = bveSwapChain_->submitCommandBuffers(&commandBuffer, &currentImageIndex_);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || bveWindow_.wasWindowResized()) {
			bveWindow_.resetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		isFrameStarted_ = false;
		currentFrameIndex_ = (currentFrameIndex_ + 1) % BveSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted_ && "Cannot call beginSwapChainRenderPass while frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on another frame's command buffer");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = bveSwapChain_->getRenderPass();
		renderPassInfo.framebuffer = bveSwapChain_->getFrameBuffer(currentImageIndex_);

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = bveSwapChain_->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {{0.01f, 0.01f, 0.01f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(bveSwapChain_->getSwapChainExtent().width);
		viewport.height = static_cast<float>(bveSwapChain_->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{0, 0}, bveSwapChain_->getSwapChainExtent()};
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void VulkanRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted_ && "Cannot call endSwapChainRenderPass while frame is not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on another frame's command buffer");

		vkCmdEndRenderPass(commandBuffer);
	}
}
