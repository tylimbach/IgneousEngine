#pragma once

#include "bve_pipeline.h"
#include "bve_model.h"
#include "frame_info.h"

#include <memory>
#include <vector>

#include "entity_manager.h"

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

		BveDevice& bveDevice;

		std::unique_ptr<BvePipeline> bvePipeline;
		VkPipelineLayout pipelineLayout;

		EntityManager& entityManager;
		std::vector<std::unique_ptr<VulkanBuffer>> uboBuffers;
	};
}
