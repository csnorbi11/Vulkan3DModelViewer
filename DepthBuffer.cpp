#include "DepthBuffer.hpp"

DepthBuffer::DepthBuffer(DeviceManager& deviceManager,
                         VkExtent2D swapchainExtent)
	:
	image(VK_NULL_HANDLE),
	imageMemory(VK_NULL_HANDLE),
	imageView(VK_NULL_HANDLE),
	swapchainExtent(swapchainExtent)
{
	create(deviceManager);
}


void DepthBuffer::create(DeviceManager& deviceManager, uint32_t width, uint32_t height)
{
	if (width != 0 && height != 0)
	{
		swapchainExtent.width = width;
		swapchainExtent.height = height;
	}

	format = findDepthFormat(deviceManager);

	createImage(swapchainExtent.width, swapchainExtent.height, 1, format, deviceManager.getSampleCount(),
	            VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
	            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory, deviceManager.getDevice(),
	            deviceManager.getPhysicalDevice());
	imageView = createImageView(image, format, VK_IMAGE_ASPECT_DEPTH_BIT, 1, deviceManager.getDevice());
}

void DepthBuffer::cleanup(VkDevice device)
{
	vkDestroyImageView(device, imageView, nullptr);
	vkDestroyImage(device, image, nullptr);
	vkFreeMemory(device, imageMemory, nullptr);
}

const VkImageView& DepthBuffer::getImageView()
{
	return imageView;
}

const VkFormat& DepthBuffer::getDepthFormat()
{
	return format;
}

VkFormat DepthBuffer::findDepthFormat(DeviceManager& deviceManager)
{
	return findSupportedFormat(deviceManager,
	                           {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT},
	                           VK_IMAGE_TILING_OPTIMAL,
	                           VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool DepthBuffer::hasScencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

VkFormat DepthBuffer::findSupportedFormat(DeviceManager& deviceManager, const std::vector<VkFormat> candidates,
                                          VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(deviceManager.getPhysicalDevice(), format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format");
}
