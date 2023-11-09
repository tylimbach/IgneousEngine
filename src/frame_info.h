#pragma once

#include "entity_manager.h"

#include <vulkan/vulkan.h>

namespace bve 
{
	struct FrameInfo 
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Entity camera;
		VkDescriptorSet globalDescriptorSet;
	};
}