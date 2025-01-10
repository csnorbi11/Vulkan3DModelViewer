#pragma once
#include "RendererCommon.h"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Texture.hpp"

class Model {
public:
	Model() = default;
	~Model() = default;

	Model(DeviceManager& deviceManager,
		VkCommandPool commandPool,
		std::vector<Vertex> vertices, std::vector<uint32_t> indices,
		std::vector<Texture> textures, std::string name);


	void cleanup(VkDevice device);

	VertexBuffer& getVertexBuffer();
	IndexBuffer& getIndexBuffer();
	std::vector<Texture>& getTextures();

	std::vector<VkDescriptorSet> descriptorSets;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	std::string name;

private:
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
	std::vector<Texture> textures;
};