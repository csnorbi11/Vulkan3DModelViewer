#include "Msaa.hpp"


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

const VkSampleCountFlagBits Msaa::getSampleCount()
{
	return sampleCount;
}

void Msaa::updateExtent(const VkExtent2D toUpdate)
{
	swapchainExtent = toUpdate;
}

void Msaa::create(uint32_t width, uint32_t height)
{
	if (width != 0 && height != 0) {
		swapchainExtent.width = width;
		swapchainExtent.height = height;
	}
	createImage(swapchainExtent.width, swapchainExtent.height, 1, swapchainImageFormat,
		sampleCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory,
		device, physicalDevice);
	imageView = createImageView(image, swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, device);
}
