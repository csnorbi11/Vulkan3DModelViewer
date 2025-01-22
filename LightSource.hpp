#pragma once
#include "RendererCommon.h"
#include "Object.hpp"
#include "Vertex.hpp"

class LightSource : public Object<VertexColor>
{
public:
	LightSource() = default;
	~LightSource() override = default;

	LightSource(DeviceManager& deviceManager,
		VkCommandPool commandPool, std::string name,
		glm::vec3 color = glm::vec3(1.0f), float intensity = 1.0f);
	glm::vec3 color;
	float intensity;

	void cleanup(VkDevice device) override;


};
