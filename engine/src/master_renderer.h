#pragma once

#include "vulkan_renderer.h"
#include "systems/render_system.h"
#include "systems/point_light_render_system.h"
#include "bve_imgui.h"

#include <vector>
#include <memory>

namespace bve
{
	class MasterRenderer
	{
	public:
		MasterRenderer(BveWindow& window, BveDevice& device, EntityManager& entityManager);
		~MasterRenderer();

		MasterRenderer(const MasterRenderer&) = delete;
		MasterRenderer& operator=(const MasterRenderer&) = delete;
		MasterRenderer(const MasterRenderer&&) = delete;
		MasterRenderer& operator=(const MasterRenderer&&) = delete;

		float getAspectRatio() const { return renderer_.getAspectRatio(); }

		bool renderFrame(float dt);

	private:
		void initGlobalDescriptorSets(); // Prepare global states like descriptor sets
		void cleanupGlobalState(); // Cleanup or update states post-rendering

		BveDevice& device_;
		VulkanRenderer renderer_;
		EntityManager& entityManager_;
		std::unique_ptr<RenderSystem> renderSystem_;
		std::unique_ptr<PointLightRenderSystem> pointLightRenderSystem_;
		BveImgui gui_;

		std::vector<std::unique_ptr<VulkanBuffer>> globalUbos_;
		std::unique_ptr<VulkanDescriptorPool> globalPool_;
		std::vector<VkDescriptorSet> globalDescriptorSets_;
		std::unique_ptr<VulkanDescriptorSetLayout> globalSetLayout_;
	};
}
