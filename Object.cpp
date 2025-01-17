#include "Object.hpp"

Object::Object(DeviceManager& deviceManager, VkCommandPool commandPool, std::vector<Vertex> vertices,
               std::vector<uint32_t> indices, std::string name)
	:
	position(glm::vec3(0.0f)),
	rotation(glm::vec3(0.0f)),
	scale(glm::vec3(1.0f)),
	name(name),
	vertexBuffer(deviceManager, commandPool, vertices),
	indexBuffer(deviceManager, commandPool, indices)
{
}

void Object::cleanup(VkDevice device)
{
	vertexBuffer.cleanup(device);
	indexBuffer.cleanup(device);
}

VertexBuffer& Object::getVertexBuffer()
{
	return vertexBuffer;
}

IndexBuffer& Object::getIndexBuffer()
{
	return indexBuffer;
}
