#pragma once
#include "RendererCommon.h"
#include "DeviceManager.hpp"

class IndexBuffer {
public:
	IndexBuffer() = default;
	~IndexBuffer() = default;

	IndexBuffer(DeviceManager& deviceManager,
		VkCommandPool commandPool,
		std::vector<uint32_t> indices);

	void cleanup(VkDevice device);

	const VkBuffer& getBuffer();
	uint32_t getCount();

private:
	std::vector<uint32_t> indices;

	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
};