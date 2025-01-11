#pragma once
#include "RendererCommon.h"
#include "Object.hpp"
#include "Texture.hpp"

class Model : public Object {
public:
	Model() = default;
	~Model() = default;

	Model(DeviceManager& deviceManager,
		VkCommandPool commandPool,
		std::vector<Vertex> vertices, std::vector<uint32_t> indices,
		std::vector<Texture> textures, std::string name);


	void cleanup(VkDevice device) override;


	std::vector<Texture>& getTextures();



private:
	std::vector<Texture> textures;
};