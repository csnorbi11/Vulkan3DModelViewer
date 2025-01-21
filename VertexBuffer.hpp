#pragma once
#include "RendererCommon.h"
#include "DeviceManager.hpp"

template <typename T>
class VertexBuffer
{
public:
	VertexBuffer() = default;
	~VertexBuffer() = default;

	VertexBuffer(DeviceManager& deviceManager, VkCommandPool& commandPool,
	             const std::vector<T>& vertices)
		:
		vertices(vertices)
	{
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();


		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, deviceManager.getDevice(),
			deviceManager.getPhysicalDevice());


		void* data;
		vkMapMemory(deviceManager.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), bufferSize);
		vkUnmapMemory(deviceManager.getDevice(), stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory, deviceManager.getDevice(),
			deviceManager.getPhysicalDevice());

		copyBuffer(stagingBuffer, buffer, bufferSize, commandPool, deviceManager.getDevice(),
			deviceManager.getGraphicsQueue());

		vkDestroyBuffer(deviceManager.getDevice(), stagingBuffer, nullptr);
		vkFreeMemory(deviceManager.getDevice(), stagingBufferMemory, nullptr);
	}

	void cleanup(VkDevice device) const
	{
		vkDestroyBuffer(device, buffer, nullptr);
		vkFreeMemory(device, bufferMemory, nullptr);
	}

	const VkBuffer& getBuffer() const
	{
		return buffer;
	}

private:
	std::vector<T> vertices;

	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
};