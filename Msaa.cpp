#include "Msaa.hpp"


Msaa::Msaa(DeviceManager& deviceManager, VkExtent2D swapchainExtent,
           VkFormat swapchainImageFormat)
	:
	sampleCount(deviceManager.getSampleCount()),
	swapchainExtent(swapchainExtent),
	swapchainImageFormat(swapchainImageFormat)
{
	create(deviceManager);
}

void Msaa::cleanup(VkDevice device)
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

void Msaa::create(DeviceManager& deviceManager,
                  uint32_t width, uint32_t height)
{
	if (width != 0 && height != 0)
	{
		swapchainExtent.width = width;
		swapchainExtent.height = height;
	}
	createImage(swapchainExtent.width, swapchainExtent.height, 1, swapchainImageFormat,
	            deviceManager.getSampleCount(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
	            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, imageMemory,
	            deviceManager.getDevice(), deviceManager.getPhysicalDevice());
	imageView = createImageView(image, swapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, deviceManager.getDevice());
}
