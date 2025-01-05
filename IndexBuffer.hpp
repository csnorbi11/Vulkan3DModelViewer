#pragma once
#include "RendererCommon.h"


class IndexBuffer {
public:
	IndexBuffer() = default;
	~IndexBuffer() = default;

	IndexBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const VkCommandPool& commandPool, const VkQueue& queue,
		const std::vector<uint32_t>& indices);

	void cleanup();

	const VkBuffer& getBuffer();
	uint32_t getCount();

private:
	std::vector<uint32_t> indices;

	VkBuffer buffer;
	VkDeviceMemory bufferMemory;

	VkDevice device;
};