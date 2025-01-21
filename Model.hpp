#pragma once
#include "RendererCommon.h"
#include "Object.hpp"
#include "Texture.hpp"



class Model : public Object<VertexNormalTexture>
{
public:
	Model() = default;
	~Model() override = default;

	Model(DeviceManager& deviceManager,
	      VkCommandPool commandPool,
	      std::vector<VertexNormalTexture> vertices, std::vector<uint32_t> indices,
	      std::vector<Texture> textures, std::string name);


	void cleanup(VkDevice device) override;


	std::vector<Texture>& getTextures();

private:
	std::vector<Texture> textures;
};
