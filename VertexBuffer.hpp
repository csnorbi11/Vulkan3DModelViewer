#pragma once
#include "RendererCommon.h"
#include "DeviceManager.hpp"


class VertexBuffer
{
public:
	VertexBuffer() = default;
	~VertexBuffer() = default;

	VertexBuffer(DeviceManager& deviceManager, VkCommandPool& commandPool,
	             const std::vector<Vertex>& vertices);

	void cleanup(VkDevice device);

	const VkBuffer& getBuffer();

private:
	std::vector<Vertex> vertices;

	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
};
