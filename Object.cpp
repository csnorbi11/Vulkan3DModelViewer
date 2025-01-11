#include "Object.hpp"

Object::Object(DeviceManager& deviceManager, VkCommandPool commandPool, std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::string name)
    :
    vertexBuffer(deviceManager, commandPool, vertices),
    indexBuffer(deviceManager, commandPool, indices),
    name(name),
    position(0.0f),
    rotation(0.0f),
    scale(1.0f)
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
