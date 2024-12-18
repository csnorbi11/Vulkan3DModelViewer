#include "Model.hpp"

Model::Model(const VkDevice& device, const VkPhysicalDevice& physicalDevice, const VkCommandPool& commandPool, const VkQueue& queue, std::vector<Vertex> vertices, std::vector<uint32_t> indices)
	:
	vertexBuffer(device,physicalDevice,commandPool,queue,vertices),
	indexBuffer(device, physicalDevice, commandPool, queue,indices)
{

}

Model::Model(const Model&& other)
	:
	vertexBuffer(other.vertexBuffer),
	indexBuffer(other.indexBuffer)
{
}

void Model::cleanup()
{
	vertexBuffer.cleanup();
	indexBuffer.cleanup();
}

VertexBuffer& Model::getVertexBuffer()
{
	return vertexBuffer;
}

IndexBuffer& Model::getIndexBuffer()
{
	return indexBuffer;
}
