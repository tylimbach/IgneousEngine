#pragma once

#include "bve_device.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <memory>

namespace bve
{
	class BveSwapChain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		BveSwapChain(BveDevice& deviceRef, VkExtent2D windowExtent);
		BveSwapChain(BveDevice& deviceRef, VkExtent2D windowExtent, std::shared_ptr<BveSwapChain> previous);
		~BveSwapChain();

		BveSwapChain(const BveSwapChain&) = delete;
		BveSwapChain& operator=(const BveSwapChain&) = delete;

		VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers_[index]; }
		VkRenderPass getRenderPass() { return renderPass_; }
		VkImageView getImageView(int index) { return swapChainImageViews_[index]; }
		size_t imageCount() { return swapChainImages_.size(); }
		VkFormat getSwapChainImageFormat() { return swapChainImageFormat_; }
		VkExtent2D getSwapChainExtent() { return swapChainExtent_; }
		uint32_t width() { return swapChainExtent_.width; }
		uint32_t height() { return swapChainExtent_.height; }

		float extentAspectRatio()
		{
			return static_cast<float>(swapChainExtent_.width) / static_cast<float>(swapChainExtent_.height);
		}

		VkFormat findDepthFormat();

		VkResult acquireNextImage(uint32_t* imageIndex);
		VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

		bool compareSwapFormats(const BveSwapChain& swapChain) const
		{
			return swapChain.swapChainDepthFormat_ == swapChainDepthFormat_ &&
				swapChain.swapChainImageFormat_ == swapChainImageFormat_;
		}

	private:
		void init();
		void createSwapChain();
		void createImageViews();
		void createDepthResources();
		void createRenderPass();
		void createFramebuffers();
		void createSyncObjects();

		// Helper functions
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkFormat swapChainImageFormat_;
		VkFormat swapChainDepthFormat_;
		VkExtent2D swapChainExtent_;

		std::vector<VkFramebuffer> swapChainFramebuffers_;
		VkRenderPass renderPass_;

		std::vector<VkImage> depthImages_;
		std::vector<VkDeviceMemory> depthImageMemorys_;
		std::vector<VkImageView> depthImageViews_;
		std::vector<VkImage> swapChainImages_;
		std::vector<VkImageView> swapChainImageViews_;

		BveDevice& device_;
		VkExtent2D windowExtent_;

		VkSwapchainKHR swapChain_;
		std::shared_ptr<BveSwapChain> oldSwapChain_;

		std::vector<VkSemaphore> imageAvailableSemaphores_;
		std::vector<VkSemaphore> renderFinishedSemaphores_;
		std::vector<VkFence> inFlightFences_;
		std::vector<VkFence> imagesInFlight_;
		size_t currentFrame_ = 0;
	};
} // namespace lve
