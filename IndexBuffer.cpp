#include "IndexBuffer.hpp"


IndexBuffer::IndexBuffer(DeviceManager& deviceManager,
		VkCommandPool commandPool,
		std::vector<uint32_t> indices)
	:
	indices(indices)
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, deviceManager.getDevice(), deviceManager.getPhysicalDevice());

	void* data;
	vkMapMemory(deviceManager.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(deviceManager.getDevice(), stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory, deviceManager.getDevice(), deviceManager.getPhysicalDevice());

	copyBuffer(stagingBuffer, buffer, bufferSize, commandPool, deviceManager.getDevice(), deviceManager.getGraphicsQueue());

	vkDestroyBuffer(deviceManager.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(deviceManager.getDevice(), stagingBufferMemory, nullptr);
}

void IndexBuffer::cleanup(VkDevice device)
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, bufferMemory, nullptr);
}

const VkBuffer& IndexBuffer::getBuffer()
{
	return buffer;
}

uint32_t IndexBuffer::getCount()
{
	return static_cast<uint32_t>(indices.size());
}
