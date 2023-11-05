#include "render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
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

	void RenderSystem::render(VkCommandBuffer commandBuffer, const Camera& camera)
	{
		bvePipeline->bind(commandBuffer);

		glm::mat4 projectionView = camera.getProjection() * camera.getView();

		EntityComponentView<RenderComponent> view = entityManager.view<RenderComponent>();
		for (const auto&& [entity, modelComponent] : view) {
			SimplePushConstantData push{};

			push.color = modelComponent.color;
			if (entityManager.hasComponent<TransformComponent>(entity)) {
				auto& transformComponent = entityManager.getComponent<TransformComponent>(entity);
				push.transform = projectionView * transformComponent.mat4();

				transformComponent.rotation.y = glm::mod(transformComponent.rotation.y + 0.0005f, glm::two_pi<float>());
				transformComponent.rotation.x = glm::mod(transformComponent.rotation.x + 0.0001f, glm::two_pi<float>());
			}

			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			modelComponent.model->bind(commandBuffer);
			modelComponent.model->draw(commandBuffer);
		}
	}
}