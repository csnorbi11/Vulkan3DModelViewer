#pragma once
#include "RendererCommon.h"


class DepthBuffer {
public:
	DepthBuffer() = default;
	~DepthBuffer() = default;

	DepthBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
		const VkExtent2D& swapchainExtent, VkSampleCountFlagBits sampleCount);

	void create(uint32_t width = 0, uint32_t height = 0);
	void cleanup();

	const VkImageView& getImageView();
	const VkFormat& getDepthFormat();


private:

	VkFormat findDepthFormat();
	bool hasScencilComponent(VkFormat format);
	VkFormat findSupportedFormat(const std::vector<VkFormat> candidates, VkImageTiling tiling,
		VkFormatFeatureFlags features);

	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;
	VkFormat format;

	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkExtent2D swapchainExtent;
	VkSampleCountFlagBits sampleCount;
};

