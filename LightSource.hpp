#pragma once
#include "RendererCommon.h"
#include "Object.hpp"

class LightSource : public Object
{
public:
	LightSource() = default;
	~LightSource() override = default;

	LightSource(DeviceManager& deviceManager,
	            VkCommandPool commandPool, std::string name,
	            glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));
	glm::vec3 color;
};
