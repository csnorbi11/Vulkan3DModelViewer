#pragma once
#include "RendererCommon.h"
#include "Model.hpp"
#include "Camera.hpp"
#include "LightSource.hpp"

#include <chrono>

const size_t MAX_LIGHTS=10;

struct alignas(16) LightStruct
{
	glm::vec3 position;
	alignas(16)glm::vec3 color;
};

struct StaticUbo
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec3 camPos;
	alignas(16)LightStruct lightSources[MAX_LIGHTS];
};

struct ModelDynamicUbo
{
	glm::mat4* model{nullptr};
};


struct UniformBuffers
{
	std::vector<VkBuffer> staticBuffers;
	std::vector<VkBuffer> dynamicBuffers;
	std::vector<VkDeviceMemory> staticBuffersMemory;
	std::vector<VkDeviceMemory> dynamicBuffersMemory;
	std::vector<void*> staticBuffersMapped;
	std::vector<void*> dynamicBuffersMapped;
};

class UniformBuffer
{
public:
	UniformBuffer() = delete;
	~UniformBuffer();

	UniformBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
	              int MAX_FRAMES_IN_FLIGHT, const VkExtent2D& swapchainExtent, VkPhysicalDeviceProperties properties,
	              const std::vector<Model>& models, const std::vector<LightSource>& lightSources,
	              const Camera& camera);

	void createDescriptorSets(Object& object, int MAX_FRAMES_IN_FLIGHT);

	void recreateDynamicBuffer();
	void calculateDynamicBuffer();
	void updateStatic(uint32_t currentFrame);
	void updateDynamic(uint32_t currentFrame);
	void cleanup();

	VkDescriptorSetLayout& getLayout();
	uint32_t getDynamicAlignment();
	VkDescriptorPool& getDescriptorPool();

private:
	void create(int MAX_FRAMES_IN_FLIGHT, const VkPhysicalDevice& physicalDevice);
	void createDescriptorSetLayout();
	void createDescriptorPool(int MAX_FRAMES_IN_FLIGHT);

	const std::vector<Model>& models;
	const std::vector<LightSource>& lightSources;

	const Camera& camera;

	uint32_t dynamicAlignment;
	size_t bufferSize;

	const uint32_t MAX_MODEL_COUNT;

	StaticUbo staticUbo;
	ModelDynamicUbo dynamicUbo;


	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayout dynamicDescriptorSetLayout;
	VkDescriptorPool descriptorPool;

	UniformBuffers uniformBuffers;


	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkExtent2D swapchainExtent;
	const int MAX_FRAMES_IN_FLIGHT;
};
