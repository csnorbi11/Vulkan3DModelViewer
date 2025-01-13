#include "VertexBuffer.hpp"

VkVertexInputBindingDescription Vertex::getBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}
std::array<VkVertexInputAttributeDescription, 4> Vertex::getAttributeDescriptions()
{
	std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, position);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, normal);

	attributeDescriptions[2].binding = 0;
	attributeDescriptions[2].location = 2;
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

	attributeDescriptions[3].binding = 0;
	attributeDescriptions[3].location = 3;
	attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[3].offset = offsetof(Vertex, color);

	return attributeDescriptions;
}

bool Vertex::operator==(const Vertex& other) const
{
	return position == other.position && texCoord == other.texCoord && normal == other.normal;
}


VertexBuffer::VertexBuffer(DeviceManager& deviceManager, VkCommandPool& commandPool,
	const std::vector<Vertex>& vertices)
	:
	vertices(vertices)
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, deviceManager.getDevice(), deviceManager.getPhysicalDevice());


	void* data;
	vkMapMemory(deviceManager.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(deviceManager.getDevice(), stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory, deviceManager.getDevice(), deviceManager.getPhysicalDevice());

	copyBuffer(stagingBuffer, buffer, bufferSize, commandPool, deviceManager.getDevice(), deviceManager.getGraphicsQueue());

	vkDestroyBuffer(deviceManager.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(deviceManager.getDevice(), stagingBufferMemory, nullptr);
}

void VertexBuffer::cleanup(VkDevice device)
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, bufferMemory, nullptr);
}

const VkBuffer& VertexBuffer::getBuffer()
{
	return buffer;
}
