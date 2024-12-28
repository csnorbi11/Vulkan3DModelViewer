#include "Model.hpp"

Model::Model(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
	const VkCommandPool& commandPool, const VkQueue& queue,
	std::vector<Vertex> vertices, std::vector<uint32_t> indices,
	std::vector<Texture> textures)
	:
	vertexBuffer(device,physicalDevice,commandPool,queue,vertices),
	indexBuffer(device, physicalDevice, commandPool, queue,indices),
	textures(textures)
{

}


void Model::cleanup()
{
	for (auto& texture : textures)
	{
		texture.cleanup();
	}
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

std::vector<Texture>& Model::getTextures()
{
	return textures;
}
