#pragma once

#include "bve_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace bve {

	class BveModel
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};

		BveModel(BveDevice &device, const Builder& builder);
		~BveModel();

		BveModel(const BveModel&) = delete;
		BveModel operator=(const BveModel&) = delete;
		BveModel(BveModel&& other) = default;
		BveModel& operator=(BveModel&& other) = default;

		void bind(VkCommandBuffer commandBuffer) const;
		void draw(VkCommandBuffer commandBuffer) const;

	private:
		void createVertexBuffer(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint32_t>& indices);

		BveDevice& bveDevice;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		bool hasIndexBuffer = false;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};
}