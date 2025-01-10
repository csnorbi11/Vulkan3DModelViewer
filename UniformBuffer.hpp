#pragma once
#include "RendererCommon.h"
#include "Model.hpp"
#include "Camera.hpp"

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
	UniformBuffer() = default;
	~UniformBuffer();

	UniformBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const int MAX_FRAMES_IN_FLIGHT, const VkExtent2D& swapchainExtent, VkPhysicalDeviceProperties properties,
		const std::vector<Model>& models, const Camera& camera);

	void createDescriptorSets(Model& model, const int MAX_FRAMES_IN_FLIGHT);

	void recreateDynamicBuffer();
	void calculateDynamicBuffer();
	void updateStatic(uint32_t currentFrame);
	void updateDynamic(uint32_t currentFrame);
	void cleanup();

	VkDescriptorSetLayout& getLayout();
	uint32_t getDynamicAlignment();
	VkDescriptorPool& getDescriptorPool();
	

	
private:
	void create(const int MAX_FRAMES_IN_FLIGHT, const VkPhysicalDevice& physicalDevice);
	void createDescriptorSetLayout();
	void createDescriptorPool(const int MAX_FRAMES_IN_FLIGHT);

	const std::vector<Model>& models;

	const Camera& camera;

	uint32_t dynamicAlignment;
	size_t bufferSize;

	const uint32_t MAX_MODEL_COUNT;

	StaticUbo staticUbo;
	DynamicUbo dynamicUbo;

	
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayout dynamicDescriptorSetLayout;
	VkDescriptorPool descriptorPool;

	UniformBuffers uniformBuffers;


	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkExtent2D swapchainExtent;
	const int MAX_FRAMES_IN_FLIGHT;
};