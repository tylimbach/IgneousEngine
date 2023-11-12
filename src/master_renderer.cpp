#include "master_renderer.h"

#include "components/components.h"

namespace bve
{
	MasterRenderer::MasterRenderer(BveWindow& window, BveDevice& device, EntityManager& entityManager) :
		device_(device),
		renderer_(window, device),
		entityManager_(entityManager),
		gui_(window, device, renderer_.getSwapChainRenderPass(), renderer_.getImageCount(), entityManager)
	{
		initGlobalDescriptorSets();
		renderSystem_ = std::make_unique<RenderSystem>(device, renderer_.getSwapChainRenderPass(), entityManager, globalSetLayout_->getDescriptorSetLayout());
		pointLightRenderSystem_ = std::make_unique<PointLightRenderSystem>(device, renderer_.getSwapChainRenderPass(), entityManager, globalSetLayout_->getDescriptorSetLayout());
	}

	MasterRenderer::~MasterRenderer() = default;

	bool MasterRenderer::renderFrame(float dt)
	{
		const VkCommandBuffer commandBuffer = renderer_.beginFrame();
		if (!commandBuffer) {
			return false;
		}

		// update global stuff
		const Entity camera = entityManager_.getOnlyEntity<ActiveCameraTag>().value();
		auto&& cameraComponent = entityManager_.getComponent<CameraComponent>(camera);
		const int frameIndex = renderer_.getFrameIndex();
		FrameInfo frameInfo{frameIndex, dt, commandBuffer, camera, globalDescriptorSets_[frameIndex]};

		GlobalUbo ubo{};
		ubo.projection = cameraComponent.projectionMatrix;
		ubo.view = cameraComponent.viewMatrix;
		ubo.inverseView = cameraComponent.inverseViewMatrix;
		pointLightRenderSystem_->update(ubo);
		globalUbos_[frameInfo.frameIndex]->writeToBuffer(&ubo);
		globalUbos_[frameInfo.frameIndex]->flush();

		// render
		gui_.newFrame();
		renderer_.beginSwapChainRenderPass(commandBuffer);

		renderSystem_->render(frameInfo);
		pointLightRenderSystem_->render(frameInfo);
		gui_.run();
		gui_.render(commandBuffer);

		renderer_.endSwapChainRenderPass(commandBuffer);
		renderer_.endFrame();

		return true;
	}

	void MasterRenderer::initGlobalDescriptorSets()
	{
		globalUbos_ = std::vector<std::unique_ptr<VulkanBuffer>>(BveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (auto& uboBuffer : globalUbos_) {
			uboBuffer = std::make_unique<VulkanBuffer>(
				device_,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				device_.properties.limits.minUniformBufferOffsetAlignment);
			uboBuffer->map();
		}

		globalPool_ = VulkanDescriptorPool::Builder(device_)
		              .setMaxSets(BveSwapChain::MAX_FRAMES_IN_FLIGHT)
		              .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, BveSwapChain::MAX_FRAMES_IN_FLIGHT)
		              .build();

		globalSetLayout_ = VulkanDescriptorSetLayout::Builder(device_)
		                   .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
		                   .build();

		globalDescriptorSets_ = std::vector<VkDescriptorSet>(BveSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets_.size(); i++) {
			auto bufferInfo = globalUbos_[i]->descriptorInfo();
			VulkanDescriptorWriter(*globalSetLayout_, *globalPool_)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets_[i]);
		}
	}
} // namespace bve
