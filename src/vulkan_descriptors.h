#pragma once

#include "bve_device.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace bve
{
	class VulkanDescriptorSetLayout
	{
	public:
		class Builder
		{
		public:
			Builder(BveDevice& bveDevice) : bveDevice{bveDevice} {}

			Builder& addBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);
			std::unique_ptr<VulkanDescriptorSetLayout> build() const;

		private:
			BveDevice& bveDevice;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		VulkanDescriptorSetLayout(BveDevice& bveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~VulkanDescriptorSetLayout();
		VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
		VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout_; }

	private:
		BveDevice& bveDevice_;
		VkDescriptorSetLayout descriptorSetLayout_;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;

		friend class VulkanDescriptorWriter;
	};

	class VulkanDescriptorPool
	{
	public:
		class Builder
		{
		public:
			Builder(BveDevice& bveDevice) : bveDevice{bveDevice} {}

			Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& setMaxSets(uint32_t count);
			std::unique_ptr<VulkanDescriptorPool> build() const;

		private:
			BveDevice& bveDevice;
			std::vector<VkDescriptorPoolSize> poolSizes{};
			uint32_t maxSets = 1000;
			VkDescriptorPoolCreateFlags poolFlags = 0;
		};

		VulkanDescriptorPool(
			BveDevice& bveDevice,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~VulkanDescriptorPool();
		VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
		VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

		bool allocateDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

		void resetPool();

	private:
		BveDevice& bveDevice_;
		VkDescriptorPool descriptorPool_;

		friend class VulkanDescriptorWriter;
	};

	class VulkanDescriptorWriter
	{
	public:
		VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool);

		VulkanDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
		VulkanDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private:
		VulkanDescriptorSetLayout& setLayout_;
		VulkanDescriptorPool& pool_;
		std::vector<VkWriteDescriptorSet> writes_;
	};
} // namespace bve
