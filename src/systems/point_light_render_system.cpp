#include "point_light_render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <thread>

#include "../bve_swap_chain.h"
#include "../entity_manager.h"
#include "../vulkan_descriptors.h"
#include "../components/components.h"

namespace bve
{
	struct PointLightPushConstants
	{
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
	};

	PointLightRenderSystem::PointLightRenderSystem(BveDevice& device, VkRenderPass renderPass, EntityManager& entityManager, VkDescriptorSetLayout globalSetLayout)
		: bveDevice_(device), entityManager_(entityManager)
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	PointLightRenderSystem::~PointLightRenderSystem()
	{
		vkDestroyPipelineLayout(bveDevice_.device(), pipelineLayout_, nullptr);
	}

	void PointLightRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);

		const std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(bveDevice_.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout_) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline info");
		}
	}

	void PointLightRenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		BvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		BvePipeline::enableAlphaBlending(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout_;
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.attributeDescriptions.clear();
		bvePipeline_ = std::make_unique<BvePipeline>(
			bveDevice_,
			"shaders/point_light.vert.spv",
			"shaders/point_light.frag.spv",
			pipelineConfig);
	}

	void PointLightRenderSystem::update(GlobalUbo& ubo) const
	{
		EntityComponentView<PointLightComponent> view = entityManager_.view<PointLightComponent>();
		int index = 0;
		for (const auto&& [entity, lightComponent] : view) {
			if (entityManager_.hasComponent<TransformComponent>(entity)) {
				assert(index < MAX_LIGHTS && "Exceeded max number of point lights");

				auto& transformComponent = entityManager_.getComponent<TransformComponent>(entity);
				auto position = glm::vec4(transformComponent.translation, 1);

				ubo.pointLights[index] = PointLight{position, lightComponent.color};
				index++;
			}
		}

		ubo.numLights = index;
	}

	void PointLightRenderSystem::render(FrameInfo& frameInfo) const
	{
		bvePipeline_->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1, &frameInfo.globalDescriptorSet, 0, nullptr);

		EntityComponentView<PointLightComponent> view = entityManager_.view<PointLightComponent>();
		for (const auto&& [entity, lightComponent] : view) {
			if (entityManager_.hasComponent<TransformComponent>(entity)) {
				PointLightPushConstants push{};

				auto& transformComponent = entityManager_.getComponent<TransformComponent>(entity);
				push.position = glm::vec4(transformComponent.translation, 1);
				push.radius = transformComponent.scale.x;
				push.color = lightComponent.color;

				vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PointLightPushConstants), &push);

				vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
			}
		}
	}
}
