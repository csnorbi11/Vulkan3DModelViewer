#include "Model.hpp"

Model::Model(DeviceManager& deviceManager,
	VkCommandPool commandPool,
	std::vector<Vertex> vertices, std::vector<uint32_t> indices,
	std::vector<Texture> textures, std::string name)
	:
	vertexBuffer(deviceManager,commandPool,vertices),
	indexBuffer(deviceManager, commandPool,indices),
	textures(textures),
	position(glm::vec3(0.0f)),
	rotation(glm::vec3(0.0f)),
	scale(glm::vec3(1.0f)),
	name(name)
{

}


void Model::cleanup(VkDevice device)
{
	for (auto& texture : textures)
	{
		texture.cleanup(device);
	}
	vertexBuffer.cleanup(device);
	indexBuffer.cleanup(device);
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
