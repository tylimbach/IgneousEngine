#pragma once

#include "bve_device.h"
#include "vulkan_buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace bve
{
	class BveModel
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void loadModel(const std::string& filepath);
		};

		BveModel(BveDevice& device, const Builder& builder);
		~BveModel();

		BveModel(const BveModel&) = delete;
		BveModel operator=(const BveModel&) = delete;
		BveModel(BveModel&& other) = default;
		BveModel& operator=(BveModel&& other) = delete;

		static std::unique_ptr<BveModel> createModelFromFile(BveDevice& device, const std::string& filepath);

		void bind(VkCommandBuffer commandBuffer) const;
		void draw(VkCommandBuffer commandBuffer) const;

	private:
		void createVertexBuffer(const std::vector<Vertex>& vertices);
		void createIndexBuffer(const std::vector<uint32_t>& indices);

		BveDevice& bveDevice_;

		std::unique_ptr<VulkanBuffer> vertexBuffer_;
		uint32_t vertexCount_;

		bool hasIndexBuffer_ = false;
		std::unique_ptr<VulkanBuffer> indexBuffer_;
		uint32_t indexCount_;
	};
}
