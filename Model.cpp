#include "Model.hpp"

Model::Model(DeviceManager& deviceManager,
	VkCommandPool commandPool,
	std::vector<Vertex> vertices, std::vector<uint32_t> indices,
	std::vector<Texture> textures, std::string name)
	:
	Object(deviceManager,commandPool,vertices,indices,name),
	textures(textures)
{

}


void Model::cleanup(VkDevice device)
{
	for (auto& texture : textures)
	{
		texture.cleanup(device);
	}
	Object::cleanup(device);
}

std::vector<Texture>& Model::getTextures()
{
	return textures;
}
