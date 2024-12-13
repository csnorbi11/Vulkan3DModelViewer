#include "SwapchainManager.hpp"



SwapchainManager::SwapchainManager()
{
}
SwapchainManager::~SwapchainManager()
{
}

SwapchainManager::SwapchainManager(const VkPhysicalDevice& phyDevice, const VkDevice& device,
	const std::vector<const char*>& deviceExtensions, const QueueFamilyIndices indices,
	VkSurfaceKHR& surface, uint32_t frameBufferWidth, uint32_t frameBufferHeight,
	VkSampleCountFlagBits sampleCount)
	:
	surface(surface),
	physicalDevice(phyDevice),
	device(device),
	extensions(extensions),
	indices(indices),
	framebufferWidth(frameBufferWidth),
	framebufferHeight(frameBufferHeight),
	sampleCount(sampleCount)
{
	create();
	depthBuffer = std::make_unique<DepthBuffer>(physicalDevice, this->device, imageExtent,sampleCount);
	msaa = std::make_unique<Msaa>(physicalDevice, this->device, sampleCount, imageExtent, imageFormat);
}

void SwapchainManager::cleanup()
{
	msaa->cleanup();
	depthBuffer->cleanup();
	
	for (uint32_t i = 0; i < imagesCount; i++) {
		vkDestroyImageView(device, imageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(device, swapChain, nullptr);

}

const VkFormat& SwapchainManager::getImageFormat()
{
	return imageFormat;
}

const VkExtent2D SwapchainManager::getImageExtent()
{
	return imageExtent;
}
const std::vector<VkImageView> SwapchainManager::getImageViews()
{
	return imageViews;
}
const DepthBuffer& SwapchainManager::getDepthBuffer()
{
	return *depthBuffer;
}
const Msaa& SwapchainManager::getMsaa()
{
	return *msaa;
}
void SwapchainManager::create()
{
	SwapChainSupportDetails swapchainSupport = querySwapChainSupport(physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presents);
	VkExtent2D extent = chooseSwapExtent(swapchainSupport.capabilities);

	uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
	if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
		imageCount = swapchainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swapCreateInfo{};
	swapCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapCreateInfo.surface = surface;
	swapCreateInfo.minImageCount = imageCount;
	swapCreateInfo.imageFormat = surfaceFormat.format;
	swapCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapCreateInfo.imageExtent = extent;
	swapCreateInfo.imageArrayLayers = 1;
	swapCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapCreateInfo.queueFamilyIndexCount = 2;
		swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapCreateInfo.queueFamilyIndexCount = 0; // Optional
		swapCreateInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	swapCreateInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;
	swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &swapCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swapchain");
	}

	imageExtent = extent;
	imagesCount = imageCount;

	createImages(imagesCount, surfaceFormat);
	createImageViews();
}

void SwapchainManager::recreate(uint32_t frameBufferWidth, uint32_t frameBufferHeight)
{

	vkDeviceWaitIdle(device);



	cleanup();

	this->framebufferWidth = frameBufferWidth;
	this->framebufferHeight = frameBufferHeight;
	create();
	

}
VkSurfaceFormatKHR SwapchainManager::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}
VkPresentModeKHR SwapchainManager::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avaiablePresentModes)
{
	for (const auto& presentMode : avaiablePresentModes) {
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return VK_PRESENT_MODE_MAILBOX_KHR;
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D SwapchainManager::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { framebufferWidth,framebufferHeight };
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

void SwapchainManager::createImages(uint32_t& imageCount, VkSurfaceFormatKHR& surfaceFormat)
{
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());

	imageFormat = surfaceFormat.format;
	imageExtent = imageExtent;
}
void SwapchainManager::createImageViews()
{
	imageViews.resize(images.size());

	for (size_t i = 0; i < imageViews.size(); i++) {
		imageViews[i] = createImageView(images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, device);
	}


}
