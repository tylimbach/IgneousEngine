#pragma once

#include "bve_window.h"
#include "bve_pipeline.h"
#include "renderer.h"
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
		static void generateVertices(int subdivisionIterations, std::vector<BveModel::Vertex>& inVertices, std::vector<BveModel::Vertex>& outVertices);

		void loadEntities();
		void createPipelineLayout();
		void createPipeline();
		void render(VkCommandBuffer commandBuffer);

		BveWindow bveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		BveDevice bveDevice{ bveWindow };
		Renderer renderer{ bveWindow, bveDevice };

		std::unique_ptr<BvePipeline> bvePipeline;
		VkPipelineLayout pipelineLayout;

		EntityManager entityManager{};
	};
}
