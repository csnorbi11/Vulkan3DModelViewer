#pragma once
#include "RendererCommon.h"
#include "Object.hpp"

class LightSource : public Object {
public:
	LightSource() = default;
	~LightSource() = default;

	LightSource(DeviceManager& deviceManager,
		VkCommandPool commandPool, std::string name);

};