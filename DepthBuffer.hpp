#pragma once
#include "RendererCommon.h"
#include "DeviceManager.hpp"

class DepthBuffer {
public:
	DepthBuffer() = default;
	~DepthBuffer() = default;

	DepthBuffer(DeviceManager& deviceManager,
		VkExtent2D swapchainExtent);

	void create(DeviceManager& deviceManager, uint32_t width = 0, uint32_t height = 0);
	void cleanup(VkDevice device);

	const VkImageView& getImageView();
	const VkFormat& getDepthFormat();


private:

	VkFormat findDepthFormat(DeviceManager& deviceManager);
	bool hasScencilComponent(VkFormat format);
	VkFormat findSupportedFormat(DeviceManager& deviceManager,const std::vector<VkFormat> candidates, VkImageTiling tiling,
		VkFormatFeatureFlags features);

	VkImage image;
	VkDeviceMemory imageMemory;
	VkImageView imageView;
	VkFormat format;

	VkExtent2D swapchainExtent;
};

