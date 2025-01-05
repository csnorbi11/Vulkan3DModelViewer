#pragma once
#include "RendererCommon.h"

class Msaa {
public:
	Msaa() = default;
	~Msaa() = default;

	Msaa(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
		VkSampleCountFlagBits sampleCount, const VkExtent2D& swapchainExtent,
		const VkFormat& swapchainImageFormat);

	void create(uint32_t width = 0, uint32_t height = 0);
	void cleanup();

	const VkImageView& getImageView();
	const VkSampleCountFlagBits getSampleCount();

	void updateExtent(const VkExtent2D toUpdate);

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