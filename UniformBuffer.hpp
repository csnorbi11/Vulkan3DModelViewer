#pragma once
#include "RendererCommon.h"

#include <chrono>

struct StaticUbo {
	glm::mat4 view;
	glm::mat4 proj;
};

struct DynamicUbo {
	glm::mat4* model{nullptr};
};

struct UniformBuffers {
	std::vector<VkBuffer> staticBuffers;
	std::vector<VkBuffer> dynamicBuffers;
	std::vector<VkDeviceMemory> staticBuffersMemory;
	std::vector<VkDeviceMemory> dynamicBuffersMemory;
	std::vector<void*> staticBuffersMapped;
	std::vector<void*> dynamicBuffersMapped;
};

class UniformBuffer {
public:
	UniformBuffer();
	~UniformBuffer();

	UniformBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const int MAX_FRAMES_IN_FLIGHT, const VkExtent2D& swapchainExtent, VkPhysicalDeviceProperties properties);

	
	void updateStatic(uint32_t currentFrame);
	void updateDynamic(uint32_t currentFrame);
	void cleanup();

	const VkDescriptorSetLayout& getLayout();
	const std::vector<VkDescriptorSet>& getSets();
	uint32_t getDynamicAlignment();

	size_t objectCount;

	
private:
	void create(const int MAX_FRAMES_IN_FLIGHT, const VkPhysicalDevice& physicalDevice);
	void createDescriptorSetLayout();
	void createDescriptorPool(const int MAX_FRAMES_IN_FLIGHT);
	void createDescriptorSets(const int MAX_FRAMES_IN_FLIGHT);

	size_t dynamicAlignment;
	size_t bufferSize;

	StaticUbo staticUbo;
	DynamicUbo dynamicUbo;

	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorSet> dynamicDescriptorSets;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayout dynamicDescriptorSetLayout;
	VkDescriptorPool descriptorPool;

	UniformBuffers uniformBuffers;


	VkDevice device;
	VkExtent2D swapchainExtent;
};