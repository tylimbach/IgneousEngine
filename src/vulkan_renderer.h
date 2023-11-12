#pragma once

#include "bve_window.h"
#include "bve_swap_chain.h"
#include "bve_model.h"

#include <memory>
#include <vector>
#include <cassert>

#include "entity_manager.h"

namespace bve
{
	class VulkanRenderer
	{
	public:
		static constexpr int WIDTH = 1600;
		static constexpr int HEIGHT = 1200;

		VulkanRenderer(BveWindow& window, BveDevice& device);
		~VulkanRenderer();

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;

		bool isFrameInProgress() const { return isFrameStarted_; }
		float getAspectRatio() const { return bveSwapChain_->extentAspectRatio(); }
		VkRenderPass getSwapChainRenderPass() const { return bveSwapChain_->getRenderPass(); }
		VkCommandBuffer getCurrentCommandBuffer() const { return commandBuffers_[currentFrameIndex_]; }
		int getFrameIndex() const { return currentFrameIndex_; }
		uint32_t getImageCount() const { return bveSwapChain_->imageCount(); }

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		BveWindow& bveWindow_;
		BveDevice& bveDevice_;
		std::unique_ptr<BveSwapChain> bveSwapChain_;
		std::vector<VkCommandBuffer> commandBuffers_;

		uint32_t currentImageIndex_;
		int currentFrameIndex_;
		bool isFrameStarted_;
	};
}
