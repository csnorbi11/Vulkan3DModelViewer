#include "IndexBuffer.hpp"

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
}

IndexBuffer::IndexBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice, const VkCommandPool& commandPool, const VkQueue& queue)
	:
	device(device)
{
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, device, physicalDevice);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory, device, physicalDevice);

	copyBuffer(stagingBuffer, buffer, bufferSize, commandPool, device, queue);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void IndexBuffer::cleanup()
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, bufferMemory, nullptr);
}

const VkBuffer& IndexBuffer::getBuffer()
{
	return buffer;
}
