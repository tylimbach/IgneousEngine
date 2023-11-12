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
	class RenderSystem
	{
	public:
		RenderSystem(BveDevice& device, VkRenderPass renderPass, EntityManager& entityManager, VkDescriptorSetLayout globalSetLayout);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		void render(FrameInfo& frameInfo) const;

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);
		void createUboBuffers();
		void createDescriptorSets();

		BveDevice& bveDevice_;

		std::unique_ptr<BvePipeline> bvePipeline_;
		VkPipelineLayout pipelineLayout_;

		EntityManager& entityManager_;
	};
}
