#include "LightSource.hpp"

LightSource::LightSource(DeviceManager& deviceManager,
	VkCommandPool commandPool, std::string name)
	:
	Object(deviceManager, commandPool,
		{
			Vertex{{-0.1f,-0.1f,0.1f},{},{},{}},
			Vertex{{-0.1f,-0.1f,-0.1f},{},{},{}},
			Vertex{{0.1f,-0.1f,0.1f},{},{},{}},
			Vertex{{0.1f,-0.1f,-0.1f},{},{},{}},
			Vertex{{-0.1f,0.1f,0.1f},{},{},{}},
			Vertex{{-0.1f,0.1f,-0.1f},{},{},{}},
			Vertex{{0.1f,0.1f,0.1f},{},{},{}},
			Vertex{{0.1f,0.1f,-0.1f},{},{},{}} },
		{
			0, 4, 6, 0, 6, 2,
			1, 3, 7, 1, 7, 5,
			0, 1, 5, 0, 5, 4,
			2, 6, 7, 2, 7, 3,
			4, 5, 7, 4, 7, 6,
			0, 2, 3, 0, 3, 1
		},
		name)
{}
