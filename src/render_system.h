#pragma once

#include "bve_pipeline.h"
#include "bve_model.h"
#include "frame_info.h"
#include "entity_manager.h"
#include "vulkan_descriptors.h"

#include <memory>
#include <vector>

namespace bve {

	class RenderSystem {
	public:
		RenderSystem(BveDevice& device, VkRenderPass renderPass, EntityManager& entityManager);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		void render(FrameInfo& frameInfo) const;

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		void createUboBuffers();
		void createDescriptorSets();

		BveDevice& bveDevice;

		std::unique_ptr<BvePipeline> bvePipeline;
		VkPipelineLayout pipelineLayout;

		std::vector<std::unique_ptr<VulkanBuffer>> uboBuffers;
		std::unique_ptr<VulkanDescriptorPool> globalPool{};
		std::vector<VkDescriptorSet> globalDescriptorSets;
		std::unique_ptr<VulkanDescriptorSetLayout> globalSetLayout;

		EntityManager& entityManager;
	};
}
