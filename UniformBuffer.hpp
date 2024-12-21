#pragma once
#include "RendererCommon.h"

#include <chrono>

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class UniformBuffer {
public:
	UniformBuffer();
	~UniformBuffer();

	UniformBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const int MAX_FRAMES_IN_FLIGHT, const VkExtent2D& swapchainExtent, VkImageView textureImageView,
		VkSampler textureSampler);

	
	void update(uint32_t currentFrame);
	void cleanup();

	const VkDescriptorSetLayout& getLayout();
	const std::vector<VkDescriptorSet>& getSets();

	
private:
	void create(const int MAX_FRAMES_IN_FLIGHT, const VkPhysicalDevice& physicalDevice);
	void createDescriptorSetLayout();
	void createDescriptorPool(const int MAX_FRAMES_IN_FLIGHT);
	void createDescriptorSets(const int MAX_FRAMES_IN_FLIGHT, VkImageView textureImageView,
		VkSampler textureSampler);


	std::vector<VkDescriptorSet> descriptorSets;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDevice device;
	VkExtent2D swapchainExtent;
};