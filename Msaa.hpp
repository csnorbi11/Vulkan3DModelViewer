#pragma once
#include "RendererCommon.h"

class Msaa {
public:
	Msaa();
	~Msaa();

	Msaa(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
		VkSampleCountFlagBits sampleCount, const VkExtent2D& swapchainExtent,
		const VkFormat& swapchainImageFormat);

	void create();
	void cleanup();

	const VkImageView& getImageView();
	const VkSampleCountFlagBits getSampleCount();

private:



	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMemory;
	VkSampleCountFlagBits sampleCount;

	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkExtent2D swapchainExtent;
	VkFormat swapchainImageFormat;
};