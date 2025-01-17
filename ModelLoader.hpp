#pragma once
#include "RendererCommon.h"
#include "Model.hpp"


#include <tiny_obj_loader.h>

class ModelLoader
{
public:
	ModelLoader() = default;
	~ModelLoader() = default;

	ModelLoader(DeviceManager& deviceManager,
	            VkCommandPool commandPool);

	Model loadModel(std::string PATH, bool verticalFlipTexture = false);

private:
	DeviceManager& deviceManager;
	VkCommandPool commandPool;
};
