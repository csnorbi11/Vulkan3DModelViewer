#pragma once
#include "RendererCommon.h"
#include "SwapchainManager.hpp"

class DepthBuffer {
public:
	DepthBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
		const VkExtent2D& swapchainExtent);
	~DepthBuffer();


	void cleanup();

private:
	void create();

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
};

