#pragma once
#include "RendererCommon.h"
#include "Model.hpp"
#include "Camera.hpp"
#include "LightSource.hpp"

#include <chrono>

constexpr size_t MAX_LIGHTS=10;

struct alignas(16) LightStruct
{
	glm::vec3 position;
	alignas(16)glm::vec3 color;
	float intensity;
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

	template <class T>
	void createDescriptorSets(Object<T>& object, int MAX_FRAMES_IN_FLIGHT)
	{
		std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT * 2);
		layouts[0] = descriptorSetLayout;
		layouts[1] = descriptorSetLayout;


		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		object.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(device, &allocInfo, object.descriptorSets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo staticBufferInfo{};
			staticBufferInfo.buffer = uniformBuffers.staticBuffers[i];
			staticBufferInfo.offset = 0;
			staticBufferInfo.range = sizeof(staticUbo);

			VkDescriptorBufferInfo modelDynamicBufferInfo{};
			modelDynamicBufferInfo.buffer = uniformBuffers.dynamicBuffers[i];
			modelDynamicBufferInfo.offset = 0;
			modelDynamicBufferInfo.range = dynamicAlignment;


			VkWriteDescriptorSet staticDescriptorWrite{};
			VkWriteDescriptorSet modelDynamicDescriptorWrite{};

			staticDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			staticDescriptorWrite.dstSet = object.descriptorSets[i];
			staticDescriptorWrite.dstBinding = 0;
			staticDescriptorWrite.dstArrayElement = 0;
			staticDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			staticDescriptorWrite.descriptorCount = 1;
			staticDescriptorWrite.pBufferInfo = &staticBufferInfo;
			staticDescriptorWrite.pImageInfo = nullptr;
			staticDescriptorWrite.pTexelBufferView = nullptr;

			modelDynamicDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			modelDynamicDescriptorWrite.dstSet = object.descriptorSets[i];
			modelDynamicDescriptorWrite.dstBinding = 1;
			modelDynamicDescriptorWrite.dstArrayElement = 0;
			modelDynamicDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			modelDynamicDescriptorWrite.descriptorCount = 1;
			modelDynamicDescriptorWrite.pBufferInfo = &modelDynamicBufferInfo;
			modelDynamicDescriptorWrite.pImageInfo = nullptr;
			modelDynamicDescriptorWrite.pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(device, 1, &staticDescriptorWrite, 0, nullptr);
			vkUpdateDescriptorSets(device, 1, &modelDynamicDescriptorWrite, 0, nullptr);

			if (typeid(object).name() != typeid(Model).name())
			{
				continue;
			}
			auto model = dynamic_cast<Model*>(&object);


			for (size_t j = 0; j < model->getTextures().size(); j++)
			{
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = model->getTextures()[j].getImageView();
				imageInfo.sampler = model->getTextures()[j].getSampler();
				VkWriteDescriptorSet textureDescriptorWrite{};
				textureDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				textureDescriptorWrite.dstSet = model->descriptorSets[i];
				textureDescriptorWrite.dstBinding = 2 + static_cast<uint32_t>(j);
				textureDescriptorWrite.dstArrayElement = 0;
				textureDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				textureDescriptorWrite.descriptorCount = 1;
				textureDescriptorWrite.pImageInfo = &imageInfo;

				vkUpdateDescriptorSets(device, 1, &textureDescriptorWrite, 0, nullptr);

			}
		}
	}

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
	void createDescriptorPool(int MAX_FRAMES_IN_FLIGHT, 
		std::vector<VkDescriptorType> descriptorTypes);

	const std::vector<Model>& models;
	const std::vector<LightSource>& lightSources;

	const Camera& camera;

	uint32_t dynamicAlignment;
	size_t bufferSize;

	const uint32_t MAX_MODEL_COUNT;
	size_t lightCount;

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
