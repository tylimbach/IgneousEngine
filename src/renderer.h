#pragma once

#include "bve_window.h"
#include "bve_swap_chain.h"
#include "bve_model.h"

#include <memory>
#include <vector>
#include <cassert>

#include "entity_manager.h"

namespace bve {

	class Renderer {

	public:
		static constexpr int WIDTH = 1600;
		static constexpr int HEIGHT = 1200;

		Renderer(BveWindow& window, BveDevice& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		bool isFrameInProgress() const { return isFrameStarted; }
		float getAspectRatio() const { return bveSwapChain->extentAspectRatio(); }
		VkRenderPass getSwapChainRenderPass() const { return bveSwapChain->getRenderPass(); }
		VkCommandBuffer getCurrentCommandBuffer() const { return commandBuffers[currentFrameIndex]; }
		int getFrameIndex() const { return currentFrameIndex; }
		uint32_t getImageCount() const { return bveSwapChain->imageCount(); }

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		BveWindow& bveWindow;
		BveDevice& bveDevice;
		std::unique_ptr<BveSwapChain> bveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
	};
}
