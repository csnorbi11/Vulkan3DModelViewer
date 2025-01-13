#pragma once
#include "RendererCommon.h"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"


class Object {
public:
	Object() = default;
	virtual ~Object() = default;

	Object(DeviceManager& deviceManager,
		VkCommandPool commandPool,
		std::vector<Vertex> vertices, std::vector<uint32_t> indices,
		std::string name);

	virtual void cleanup(VkDevice device);

	VertexBuffer& getVertexBuffer();
	IndexBuffer& getIndexBuffer();

	std::vector<VkDescriptorSet> descriptorSets;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	std::string name;
protected:
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;
};