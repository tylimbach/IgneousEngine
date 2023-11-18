#pragma once

#include "../bve_pipeline.h"
#include "../bve_model.h"
#include "../frame_info.h"
#include "../entity_manager.h"

#include <memory>

namespace bve
{
	class PointLightRenderSystem
	{
	public:
		PointLightRenderSystem(BveDevice& device, VkRenderPass renderPass, EntityManager& entityManager, VkDescriptorSetLayout globalSetLayout);
		~PointLightRenderSystem();

		PointLightRenderSystem(const PointLightRenderSystem&) = delete;
		PointLightRenderSystem& operator=(const PointLightRenderSystem&) = delete;
		PointLightRenderSystem(const PointLightRenderSystem&&) = delete;
		PointLightRenderSystem& operator=(const PointLightRenderSystem&&) = delete;

		void update(GlobalUbo& ubo) const;
		void render(FrameInfo& frameInfo) const;

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		BveDevice& bveDevice_;

		std::unique_ptr<BvePipeline> bvePipeline_;
		VkPipelineLayout pipelineLayout_;

		EntityManager& entityManager_;
	};
}
