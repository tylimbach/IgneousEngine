#include "render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <thread>

#include "entity_manager.h"
#include "components/components.h"

namespace bve {

	struct SimplePushConstantData {
		glm::mat4 transform{ 1.0f };
		alignas(16) glm::vec3 color;
	};

	RenderSystem::RenderSystem(BveDevice& device, VkRenderPass renderPass, EntityManager& entityManager) : bveDevice(device), entityManager(entityManager) {
		createPipelineLayout();
		createPipeline(renderPass);
	}

	RenderSystem::~RenderSystem() {
		vkDestroyPipelineLayout(bveDevice.device(), pipelineLayout, nullptr);
	}

	void RenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(bveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline info");
		}
	}

	void RenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		BvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		bvePipeline = std::make_unique<BvePipeline>(
			bveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void RenderSystem::render(VkCommandBuffer commandBuffer)
	{
		bvePipeline->bind(commandBuffer);

		EntityComponentView<RenderComponent> view = entityManager.view<RenderComponent>();
		for (const auto&& [entity, modelComponent] : view) {
			SimplePushConstantData push{};

			push.color = modelComponent.color;
			if (entityManager.hasComponent<TransformComponent>(entity)) {
				auto& transformComponent = entityManager.getComponent<TransformComponent>(entity);
				push.transform = transformComponent.mat4();

				transformComponent.rotation.y = glm::mod(transformComponent.rotation.y + 0.001f, glm::two_pi<float>());
			}

			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			modelComponent.model->bind(commandBuffer);
			modelComponent.model->draw(commandBuffer);
		}
	}
}