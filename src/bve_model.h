#pragma once

#include "bve_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace bve {

	class BveModel {
	public:
		struct Vertex {
			glm::vec2 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		BveModel(BveDevice &device, const std::vector<Vertex> &vertices);
		~BveModel();

		BveModel(const BveModel&) = delete;
		BveModel operator=(const BveModel&) = delete;
		BveModel(BveModel&& other) = default;
		BveModel& operator=(BveModel&& other) = default;

		void bind(VkCommandBuffer commandBuffer) const;
		void draw(VkCommandBuffer commandBuffer) const;

	private:
		void createVertexBuffers(const std::vector<Vertex>& vertices);

		BveDevice& bveDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}