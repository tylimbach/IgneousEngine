#pragma once

#include "../bve_pipeline.h"
#include "../bve_model.h"
#include "../frame_info.h"
#include "../entity_manager.h"
#include "../vulkan_descriptors.h"

#include <memory>
#include <vector>

namespace bve
{
	class PointLightRenderSystem
	{
	public:
		PointLightRenderSystem(BveDevice& device, VkRenderPass renderPass, EntityManager& entityManager);
		~PointLightRenderSystem();

		PointLightRenderSystem(const PointLightRenderSystem&) = delete;
		PointLightRenderSystem& operator=(const PointLightRenderSystem&) = delete;

		void render(FrameInfo& frameInfo) const;

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);
		void createUboBuffers();
		void createDescriptorSets();

		BveDevice& bveDevice_;

		std::unique_ptr<BvePipeline> bvePipeline_;
		VkPipelineLayout pipelineLayout_;

		std::vector<std::unique_ptr<VulkanBuffer>> uboBuffers_;
		std::unique_ptr<VulkanDescriptorPool> globalPool_{};
		std::vector<VkDescriptorSet> globalDescriptorSets_;
		std::unique_ptr<VulkanDescriptorSetLayout> globalSetLayout_;

		EntityManager& entityManager_;
	};
}
