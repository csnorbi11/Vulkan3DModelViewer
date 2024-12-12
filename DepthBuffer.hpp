#pragma once
#include "RendererCommon.h"


class DepthBuffer {
public:
	DepthBuffer();
	~DepthBuffer();

	DepthBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
		const VkExtent2D& swapchainExtent, VkSampleCountFlagBits sampleCount);

	void create();
	void cleanup();

private:

	VkFormat findDepthFormat();
	bool hasScencilComponent(VkFormat format);
	VkFormat findSupportedFormat(const std::vector<VkFormat> candidates, VkImageTiling tiling,
		VkFormatFeatureFlags features);

	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;

	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkExtent2D swapchainExtent;
	VkSampleCountFlagBits sampleCount;
};

