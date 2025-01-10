#pragma once
#include "RendererCommon.h"
#include "DeviceManager.hpp"

class Msaa {
public:
	Msaa() = default;
	~Msaa() = default;

	Msaa(DeviceManager& deviceManager, VkExtent2D swapchainExtent,
		VkFormat swapchainImageFormat);

	void create(DeviceManager& deviceManager,
		uint32_t width = 0, uint32_t height = 0);
	void cleanup(VkDevice device);

	const VkImageView& getImageView();
	const VkSampleCountFlagBits getSampleCount();

	void updateExtent(const VkExtent2D toUpdate);

private:



	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMemory;
	VkSampleCountFlagBits sampleCount;

	VkExtent2D swapchainExtent;
	VkFormat swapchainImageFormat;
};