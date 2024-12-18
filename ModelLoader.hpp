#pragma once
#include "RendererCommon.h"
#include "Model.hpp"


#include <tiny_obj_loader.h>

class ModelLoader {
public:
	ModelLoader()=default;
	~ModelLoader();

	ModelLoader(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const VkCommandPool& commandPool, const VkQueue& queue,
		std::shared_ptr<std::vector<Model>> models);

	void loadModel(const std::string PATH);
	void cleanup();

	std::shared_ptr<std::vector<Model>> models;
private:


	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;
	VkQueue queue;

};