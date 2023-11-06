#pragma once

#include "bve_pipeline.h"
#include "bve_model.h"
#include "camera.h"

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

		void render(VkCommandBuffer commandBuffer, Entity activeCamera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		BveDevice& bveDevice;

		std::unique_ptr<BvePipeline> bvePipeline;
		VkPipelineLayout pipelineLayout;

		EntityManager& entityManager;
	};
}
