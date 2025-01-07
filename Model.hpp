#pragma once
#include "RendererCommon.h"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Texture.hpp"

class Model {
public:
	Model() = default;
	~Model() = default;

	Model(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const VkCommandPool& commandPool, const VkQueue& queue,
		std::vector<Vertex> vertices, std::vector<uint32_t> indices,
		std::vector<Texture> textures, std::string name);


	void cleanup();

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