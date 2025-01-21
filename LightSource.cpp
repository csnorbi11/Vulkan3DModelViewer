#include "LightSource.hpp"

LightSource::LightSource(DeviceManager& deviceManager,
	VkCommandPool commandPool, std::string name,
	glm::vec3 color, float intensity)
	:
	Object<VertexColor>(deviceManager, commandPool,
		{
			VertexColor{{-0.1f, -0.1f, 0.1f},  {color}},
			VertexColor{{-0.1f, -0.1f, -0.1f},  {color}},
			VertexColor{{0.1f, -0.1f, 0.1f},  {color}},
			VertexColor{{0.1f, -0.1f, -0.1f}, {color}},
			VertexColor{{-0.1f, 0.1f, 0.1f}, {color}},
			VertexColor{{-0.1f, 0.1f, -0.1f},  {color}},
			VertexColor{{0.1f, 0.1f, 0.1f},  {color}},
			VertexColor{{0.1f, 0.1f, -0.1f},  {color}}
		},
		   {
			   6, 4, 0, 2, 6, 0,
			   7, 3, 1, 5, 7, 1,
			   5, 1, 0, 4, 5, 0,
			   7, 6, 2, 3, 7, 2,
			   7, 5, 4, 6, 7, 4,
			   3, 2, 0, 1, 3, 0
		   },
		name),
	color(color),
	intensity(intensity)
{
}

void LightSource::cleanup(VkDevice device)
{
	Object<VertexColor>::cleanup(device);
}
