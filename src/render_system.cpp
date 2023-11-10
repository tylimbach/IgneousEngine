#include "render_system.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <thread>

#include "bve_swap_chain.h"
#include "entity_manager.h"
#include "vulkan_descriptors.h"
#include "components/components.h"

namespace bve
{
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{1.f};
		glm::mat4 normalMatrix{1.f};
	};

	struct GlobalUbo
	{
		glm::mat4 projectionView{1.f};
		glm::vec4 ambientLightColor{1.f, 1.f, 1.f, .2f};
		glm::vec3 lightPosition{-1.f};
		alignas(16) glm::vec4 lightColor{1.f};
	};

	RenderSystem::RenderSystem(BveDevice& device, VkRenderPass renderPass, EntityManager& entityManager)
		: bveDevice_(device), entityManager_(entityManager)
	{
		createUboBuffers();
		createDescriptorSets();
		createPipelineLayout();
		createPipeline(renderPass);
	}

	RenderSystem::~RenderSystem()
	{
		vkDestroyPipelineLayout(bveDevice_.device(), pipelineLayout_, nullptr);
	}

	void RenderSystem::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		const std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout_->getDescriptorSetLayout()};

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

	void RenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		BvePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout_;
		bvePipeline_ = std::make_unique<BvePipeline>(
			bveDevice_,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void RenderSystem::createUboBuffers()
	{
		uboBuffers_ = std::vector<std::unique_ptr<VulkanBuffer>>(BveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (auto& uboBuffer : uboBuffers_) {
			uboBuffer = std::make_unique<VulkanBuffer>(
				bveDevice_,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				bveDevice_.properties.limits.minUniformBufferOffsetAlignment);
			uboBuffer->map();
		}
	}

	void RenderSystem::createDescriptorSets()
	{
		globalPool_ = VulkanDescriptorPool::Builder(bveDevice_)
		             .setMaxSets(BveSwapChain::MAX_FRAMES_IN_FLIGHT)
		             .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, BveSwapChain::MAX_FRAMES_IN_FLIGHT)
		             .build();

		globalSetLayout_ = VulkanDescriptorSetLayout::Builder(bveDevice_)
		                  .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
		                  .build();

		globalDescriptorSets_ = std::vector<VkDescriptorSet>(BveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets_.size(); i++) {
			auto bufferInfo = uboBuffers_[i]->descriptorInfo();
			VulkanDescriptorWriter(*globalSetLayout_, *globalPool_)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets_[i]);
		}
	}


	void RenderSystem::render(FrameInfo& frameInfo) const
	{
		bvePipeline_->bind(frameInfo.commandBuffer);

		auto&& cameraComp = entityManager_.getComponent<CameraComponent>(frameInfo.camera);
		const glm::mat4 projectionView = cameraComp.projectionMatrix * cameraComp.viewMatrix;

		GlobalUbo ubo{};
		ubo.projectionView = projectionView;
		uboBuffers_[frameInfo.frameIndex]->writeToBuffer(&ubo);
		uboBuffers_[frameInfo.frameIndex]->flush();

		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout_, 0, 1, &globalDescriptorSets_[frameInfo.frameIndex], 0, nullptr);

		EntityComponentView<RenderComponent> view = entityManager_.view<RenderComponent>();
		for (const auto&& [entity, modelComponent] : view) {
			SimplePushConstantData push{};

			if (entityManager_.hasComponent<TransformComponent>(entity)) {
				auto& transformComponent = entityManager_.getComponent<TransformComponent>(entity);
				push.modelMatrix = transformComponent.mat4();
				push.normalMatrix = transformComponent.normalMatrix();
			}

			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout_, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			modelComponent.model->bind(frameInfo.commandBuffer);
			modelComponent.model->draw(frameInfo.commandBuffer);
		}
	}
}
