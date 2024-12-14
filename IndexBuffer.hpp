#pragma once
#include "RendererCommon.h"


class IndexBuffer {
public:
	IndexBuffer();
	~IndexBuffer();

	IndexBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const VkCommandPool& commandPool, const VkQueue& queue);

	void cleanup();

	const VkBuffer& getBuffer();

private:
	std::vector<uint32_t> indices;

	VkBuffer buffer;
	VkDeviceMemory bufferMemory;

	VkDevice device;
};