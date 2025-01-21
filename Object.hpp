#pragma once
#include "RendererCommon.h"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

template <typename T>
class Object
{
public:
	Object() = default;
	virtual ~Object() = default;

	Object(DeviceManager& deviceManager,
	       VkCommandPool commandPool,
	       std::vector<T> vertices, std::vector<uint32_t> indices,
	       std::string name)
		:
		position(glm::vec3(0.0f)),
		rotation(glm::vec3(0.0f)),
		scale(glm::vec3(1.0f)),
		name(name),
		vertexBuffer(deviceManager, commandPool, vertices),
		indexBuffer(deviceManager, commandPool, indices)
	{
	}

	virtual void cleanup(VkDevice device)
	{
		vkDeviceWaitIdle(device);
		vertexBuffer.cleanup(device);
		indexBuffer.cleanup(device);
	}

	VertexBuffer<T>& getVertexBuffer()
	{
		return vertexBuffer;
	}
	IndexBuffer& getIndexBuffer()
	{
		return indexBuffer;
	}

	std::vector<VkDescriptorSet> descriptorSets;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	std::string name;

private:
	VertexBuffer<T> vertexBuffer;
	IndexBuffer indexBuffer;
};