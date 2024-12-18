#pragma once
#include "RendererCommon.h"



class VertexBuffer {
public:
	VertexBuffer();
	~VertexBuffer();

	VertexBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const VkCommandPool& commandPool, const VkQueue& queue,
		const std::vector<Vertex>& vertices);

	void cleanup();

	const VkBuffer& getBuffer();

private:
	std::vector<Vertex> vertices;

	VkBuffer buffer;
	VkDeviceMemory bufferMemory;

	VkDevice device;
};