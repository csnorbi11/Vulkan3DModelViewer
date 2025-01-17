#pragma once
#include "RendererCommon.h"


class SyncObjects
{
public:
	SyncObjects() = default;
	~SyncObjects() = default;

	SyncObjects(VkDevice device, int MAX_FRAMES_IN_FLIGHT);

	void cleanup(VkDevice device);

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
};
