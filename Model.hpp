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
		std::vector<Texture> textures);


	void cleanup();

	VertexBuffer& getVertexBuffer();
	IndexBuffer& getIndexBuffer();

private:
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
	std::vector<Texture> textures;
};