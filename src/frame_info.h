#pragma once

#include "entity_manager.h"

#include <vulkan/vulkan.h>

namespace bve
{
#define MAX_LIGHTS 10

	struct PointLight
	{
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct GlobalUbo
	{
		glm::mat4 projection{1.f};
		glm::mat4 view{1.f};
		glm::vec4 ambientLightColor{1.f, .7f, .7f, .04f};
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Entity camera;
		VkDescriptorSet globalDescriptorSet;
	};
}
