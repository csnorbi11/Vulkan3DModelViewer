#pragma once
#include "RendererCommon.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
	bool operator==(const Vertex& other) const;
};

class VertexBuffer {
public:
	VertexBuffer();
	~VertexBuffer();

	VertexBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const VkCommandPool& commandPool, const VkQueue& queue);

	void cleanup();

private:
	std::vector<Vertex> vertices;

	VkBuffer buffer;
	VkDeviceMemory bufferMemory;

	VkDevice device;
};