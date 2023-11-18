#include "pch.h"
#include "vulkan_descriptors.h"

#include <stdexcept>

namespace bve
{
	// *************** Descriptor Set Layout Builder *********************

	VulkanDescriptorSetLayout::Builder& VulkanDescriptorSetLayout::Builder::addBinding(
		uint32_t binding,
		VkDescriptorType descriptorType,
		VkShaderStageFlags stageFlags,
		uint32_t count)
	{
		assert(bindings.count(binding) == 0 && "Binding already in use");
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		bindings[binding] = layoutBinding;
		return *this;
	}

	std::unique_ptr<VulkanDescriptorSetLayout> VulkanDescriptorSetLayout::Builder::build() const
	{
		return std::make_unique<VulkanDescriptorSetLayout>(bveDevice, bindings);
	}

	// *************** Descriptor Set Layout *********************

	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(
		BveDevice& bveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
		: bveDevice_{bveDevice}, bindings_{bindings}
	{
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		for (auto kv : bindings) {
			setLayoutBindings.push_back(kv.second);
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(
			bveDevice.device(),
			&descriptorSetLayoutInfo,
			nullptr,
			&descriptorSetLayout_) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(bveDevice_.device(), descriptorSetLayout_, nullptr);
	}

	// *************** Descriptor Pool Builder *********************

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::addPoolSize(
		VkDescriptorType descriptorType, uint32_t count)
	{
		poolSizes.push_back({descriptorType, count});
		return *this;
	}

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::setPoolFlags(
		VkDescriptorPoolCreateFlags flags)
	{
		poolFlags = flags;
		return *this;
	}

	VulkanDescriptorPool::Builder& VulkanDescriptorPool::Builder::setMaxSets(uint32_t count)
	{
		maxSets = count;
		return *this;
	}

	std::unique_ptr<VulkanDescriptorPool> VulkanDescriptorPool::Builder::build() const
	{
		return std::make_unique<VulkanDescriptorPool>(bveDevice, maxSets, poolFlags, poolSizes);
	}

	// *************** Descriptor Pool *********************

	VulkanDescriptorPool::VulkanDescriptorPool(
		BveDevice& bveDevice,
		uint32_t maxSets,
		VkDescriptorPoolCreateFlags poolFlags,
		const std::vector<VkDescriptorPoolSize>& poolSizes)
		: bveDevice_{bveDevice}
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		if (vkCreateDescriptorPool(bveDevice.device(), &descriptorPoolInfo, nullptr, &descriptorPool_) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		vkDestroyDescriptorPool(bveDevice_.device(), descriptorPool_, nullptr);
	}

	bool VulkanDescriptorPool::allocateDescriptorSet(
		const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool_;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

		// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
		// a new pool whenever an old pool fills up. But this is beyond our current scope
		if (vkAllocateDescriptorSets(bveDevice_.device(), &allocInfo, &descriptor) != VK_SUCCESS) {
			return false;
		}
		return true;
	}

	void VulkanDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
	{
		vkFreeDescriptorSets(
			bveDevice_.device(),
			descriptorPool_,
			descriptors.size(),
			descriptors.data());
	}

	void VulkanDescriptorPool::resetPool()
	{
		vkResetDescriptorPool(bveDevice_.device(), descriptorPool_, 0);
	}

	// *************** Descriptor Writer *********************

	VulkanDescriptorWriter::VulkanDescriptorWriter(VulkanDescriptorSetLayout& setLayout, VulkanDescriptorPool& pool)
		: setLayout_{setLayout}, pool_{pool} {}

	VulkanDescriptorWriter& VulkanDescriptorWriter::writeBuffer(
		uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
	{
		assert(setLayout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = setLayout_.bindings_[binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;

		writes_.push_back(write);
		return *this;
	}

	VulkanDescriptorWriter& VulkanDescriptorWriter::writeImage(
		uint32_t binding, VkDescriptorImageInfo* imageInfo)
	{
		assert(setLayout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = setLayout_.bindings_[binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;

		writes_.push_back(write);
		return *this;
	}

	bool VulkanDescriptorWriter::build(VkDescriptorSet& set)
	{
		bool success = pool_.allocateDescriptorSet(setLayout_.getDescriptorSetLayout(), set);
		if (!success) {
			return false;
		}
		overwrite(set);
		return true;
	}

	void VulkanDescriptorWriter::overwrite(VkDescriptorSet& set)
	{
		for (auto& write : writes_) {
			write.dstSet = set;
		}
		vkUpdateDescriptorSets(pool_.bveDevice_.device(), writes_.size(), writes_.data(), 0, nullptr);
	}
} // namespace bve
