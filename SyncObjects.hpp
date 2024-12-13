#pragma once
#include "RendererCommon.h"



class SyncObjects {
public:
	SyncObjects();
	~SyncObjects();

	SyncObjects(const VkDevice& device, const int MAX_FRAMES_IN_FLIGHT);

	void cleanup();

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;


private:
	VkDevice device;
};