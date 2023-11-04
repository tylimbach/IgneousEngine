#pragma once

#include "bve_window.h"
#include "bve_pipeline.h"
#include "bve_swap_chain.h"
#include "bve_model.h"

#include <memory>
#include <vector>

#include "entity_manager.h"

namespace bve {

	class BasicApp {

	public:
		static constexpr int WIDTH = 1600;
		static constexpr int HEIGHT = 1200;

		BasicApp();
		~BasicApp();

		BasicApp(const BasicApp&) = delete;
		BasicApp& operator=(const BasicApp&) = delete;

		void run();

	private:
		void loadEntities();
		static void pollGlfw(bool &stop);
		static void generateVertices(int subdivisionIterations, std::vector<BveModel::Vertex> &inVertices, std::vector<BveModel::Vertex> &outVertices);
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int index);
		void freeCommandBuffers();
		void render(VkCommandBuffer commandBuffer);

		BveWindow bveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		BveDevice bveDevice{ bveWindow };
		EntityManager entityManager{};
		std::unique_ptr<BveSwapChain> bveSwapChain;
		std::unique_ptr<BvePipeline> bvePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}
