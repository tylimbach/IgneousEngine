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

namespace bve {

	struct SimplePushConstantData {
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	struct GlobalUbo
	{
		alignas(16) glm::mat4 projectionView{ 1.f };
		alignas(16) glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};

	RenderSystem::RenderSystem(BveDevice& device, VkRenderPass renderPass, EntityManager& entityManager)
		: bveDevice(device), entityManager(entityManager) {
		createUboBuffers();
		createDescriptorSets();
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

		const std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout->getDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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

	void RenderSystem::createUboBuffers()
	{
		uboBuffers = std::vector<std::unique_ptr<VulkanBuffer>>(BveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (auto& uboBuffer : uboBuffers) {
			uboBuffer = std::make_unique<VulkanBuffer>(
				bveDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				bveDevice.properties.limits.minUniformBufferOffsetAlignment);
			uboBuffer->map();
		}
	}

	void RenderSystem::createDescriptorSets()
	{
		globalPool = VulkanDescriptorPool::Builder(bveDevice)
			.setMaxSets(BveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, BveSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();

		globalSetLayout = VulkanDescriptorSetLayout::Builder(bveDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.build();

		globalDescriptorSets = std::vector<VkDescriptorSet>(BveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++) {
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			VulkanDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}
	}


	void RenderSystem::render(FrameInfo& frameInfo) const
	{
		bvePipeline->bind(frameInfo.commandBuffer);

		auto&& cameraComp = entityManager.get<CameraComponent>(frameInfo.camera);
		const glm::mat4 projectionView = cameraComp.projectionMatrix * cameraComp.viewMatrix;

		GlobalUbo ubo{};
		ubo.projectionView = projectionView;
		uboBuffers[frameInfo.frameIndex]->writeToBuffer(&ubo);
		uboBuffers[frameInfo.frameIndex]->flush();

		vkCmdBindDescriptorSets(frameInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &globalDescriptorSets[frameInfo.frameIndex], 0, nullptr);

		EntityComponentView<RenderComponent> view = entityManager.view<RenderComponent>();
		for (const auto&& [entity, modelComponent] : view) {
			SimplePushConstantData push{};

			if (entityManager.has<TransformComponent>(entity)) {
				auto& transformComponent = entityManager.get<TransformComponent>(entity);
				push.modelMatrix = transformComponent.mat4();
				push.normalMatrix = transformComponent.normalMatrix();
			}

			vkCmdPushConstants(frameInfo.commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			modelComponent.model->bind(frameInfo.commandBuffer);
			modelComponent.model->draw(frameInfo.commandBuffer);
		}
	}
}