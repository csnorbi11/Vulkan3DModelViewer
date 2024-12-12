#include "Msaa.hpp"

Msaa::Msaa()
{
}
Msaa::~Msaa()
{
}

Msaa::Msaa(const VkPhysicalDevice& physicalDevice, const VkDevice& device,
	VkSampleCountFlagBits sampleCount, const VkExtent2D& swapchainExtent,
	const VkFormat& swapchainImageFormat)
	:
	physicalDevice(physicalDevice),
	device(device),
	swapchainExtent(swapchainExtent),
	swapchainImageFormat(swapchainImageFormat),
	sampleCount(sampleCount)
{
	create();
}

void Msaa::cleanup()
{
	vkDestroyImageView(device, imageView, nullptr);
	vkDestroyImage(device, image, nullptr);
	vkFreeMemory(device, imageMemory, nullptr);
}

const VkImageView& Msaa::getImageView()
{
	return imageView;
}

void Msaa::create()
{
	createImage(swapchainExtent.width, swapchainExtent.height, 1, swapchainImageFormat,
		sampleCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory,
		device, physicalDevice);
}
