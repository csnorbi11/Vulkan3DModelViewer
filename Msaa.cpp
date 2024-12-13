#include "Msaa.hpp"

Msaa::Msaa()
	:
	image(VK_NULL_HANDLE),
	imageMemory(VK_NULL_HANDLE),
	imageView(VK_NULL_HANDLE),
	sampleCount(VK_SAMPLE_COUNT_1_BIT),
	physicalDevice(VK_NULL_HANDLE),
	device(VK_NULL_HANDLE),
	swapchainExtent({ 0,0 }),
	swapchainImageFormat(VK_FORMAT_UNDEFINED)
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

const VkSampleCountFlagBits Msaa::getSampleCount()
{
	return sampleCount;
}

void Msaa::create()
{
	createImage(swapchainExtent.width, swapchainExtent.height, 1, swapchainImageFormat,
		sampleCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
		VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory,
		device, physicalDevice);
	imageView = createImageView(image, swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, device);
}
