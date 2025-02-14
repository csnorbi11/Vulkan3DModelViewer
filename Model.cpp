#include "Model.hpp"

Model::Model(DeviceManager& deviceManager,
             VkCommandPool commandPool,
             std::vector<VertexNormalTexture> vertices, std::vector<uint32_t> indices,
             std::vector<Texture> textures, std::string name)
	:
	Object<VertexNormalTexture>(deviceManager, commandPool, vertices, indices, name),
	textures(textures)
{
}


void Model::cleanup(VkDevice device)
{

	Object<VertexNormalTexture>::cleanup(device);
	for (auto& texture : textures)
	{
		texture.cleanup(device);
	}
}

std::vector<Texture>& Model::getTextures()
{
	return textures;
}
