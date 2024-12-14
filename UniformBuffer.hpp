#pragma once
#include "RendererCommon.h"

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class UniformBuffer {
public:
	UniformBuffer();
	~UniformBuffer();

	UniformBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice, unsigned int MAX_FRAMES_IN_FLIGHT);

	void update();

private:
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDevice device;

};