#pragma once
#include "RendererCommon.h"
#include "Model.hpp"


#include <tiny_obj_loader.h>

class ModelLoader {
public:
	ModelLoader() = default;
	~ModelLoader() = default;

	ModelLoader(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const VkCommandPool& commandPool, const VkQueue& queue,
		const VkPhysicalDeviceProperties& properties);

	Model loadModel(const std::string PATH, bool verticalFlipTexture = false);


private:


	VkDevice device;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;
	VkQueue queue;

};