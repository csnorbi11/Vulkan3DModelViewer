#include "DepthBuffer.hpp"

DepthBuffer::DepthBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
	const VkExtent2D& swapchainExtent, VkSampleCountFlagBits sampleCount)
	:
	image(VK_NULL_HANDLE),
	imageMemory(VK_NULL_HANDLE),
	imageView(VK_NULL_HANDLE),
	physicalDevice(physicalDevice),
	device(device),
	swapchainExtent(swapchainExtent),
	sampleCount(sampleCount)
{
	create();
}
DepthBuffer::~DepthBuffer()
{
}

void DepthBuffer::create()
{
	VkFormat depthFormat = findDepthFormat();

	createImage(swapchainExtent.width, swapchainExtent.height, 1, depthFormat, sampleCount,
		VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory, device, physicalDevice);
	imageView = createImageView(image, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, device);
}

void DepthBuffer::cleanup()
{
	vkDestroyImageView(device, imageView, nullptr);
	vkDestroyImage(device, image, nullptr);
	vkFreeMemory(device, imageMemory, nullptr);
}

VkFormat DepthBuffer::findDepthFormat()
{
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT,VK_FORMAT_D32_SFLOAT_S8_UINT,VK_FORMAT_D16_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}
bool DepthBuffer::hasScencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}
VkFormat DepthBuffer::findSupportedFormat(const std::vector<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format");
}
