#include "SwapchainManager.hpp"


SwapchainManager::SwapchainManager(DeviceManager& deviceManager,
	VkSurfaceKHR surface, uint32_t frameBufferWidth, uint32_t frameBufferHeight)
	:
	surface(surface),
	deviceManager(deviceManager),
	framebufferWidth(framebufferWidth),
	framebufferHeight(frameBufferHeight)
{
	create();

	depthBuffer = std::make_unique<DepthBuffer>(deviceManager, imageExtent);
	msaa = std::make_unique<Msaa>(deviceManager, imageExtent, imageFormat);
	renderpass = std::make_unique<RenderPass>(deviceManager.getDevice(), imageFormat, deviceManager.getSampleCount(), depthBuffer->getDepthFormat());
	framebuffer = std::make_unique<Framebuffer>(deviceManager.getDevice(), imageViews, msaa->getImageView(), depthBuffer->getImageView(),
		renderpass->getRenderPass(), imageExtent);
}

void SwapchainManager::cleanup()
{
	framebuffer->cleanup(deviceManager.getDevice());

	msaa->cleanup(deviceManager.getDevice());
	depthBuffer->cleanup(deviceManager.getDevice());
	
	for (uint32_t i = 0; i < imagesCount; i++) {
		vkDestroyImageView(deviceManager.getDevice(), imageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(deviceManager.getDevice(), swapChain, nullptr);


	renderpass->cleanup(deviceManager.getDevice());
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
const VkSwapchainKHR& SwapchainManager::getSwapchain()
{
	return swapChain;
}
Msaa& SwapchainManager::getMsaa()
{
	return *msaa;
}
DepthBuffer& SwapchainManager::getDepthBuffer()
{
	return *depthBuffer;
}
RenderPass& SwapchainManager::getRenderPass()
{
	return *renderpass;
}
Framebuffer& SwapchainManager::getFramebuffer()
{
	return *framebuffer;
}
void SwapchainManager::create()
{
	SwapChainSupportDetails swapchainSupport = querySwapChainSupport(deviceManager.getPhysicalDevice(), surface);

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


	uint32_t queueFamilyIndices[] = { deviceManager.getIndices().graphicsFamily.value(), deviceManager.getIndices().presentFamily.value()};

	if (deviceManager.getIndices().graphicsFamily != deviceManager.getIndices().presentFamily) {
		swapCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapCreateInfo.queueFamilyIndexCount = 2;
		swapCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		swapCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	swapCreateInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;
	swapCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(deviceManager.getDevice(), &swapCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swapchain");
	}

	imageFormat = surfaceFormat.format;
	imageExtent = extent;

	createImages(imagesCount, surfaceFormat);
	createImageViews();
}

void SwapchainManager::recreate(uint32_t frameBufferWidth, uint32_t frameBufferHeight)
{

	this->framebufferWidth = frameBufferWidth;
	this->framebufferHeight = frameBufferHeight;
	create();
	msaa->create(deviceManager, frameBufferWidth, frameBufferHeight);
	depthBuffer->create(deviceManager,frameBufferWidth, frameBufferHeight);
	renderpass->create(deviceManager.getDevice(), deviceManager.getSampleCount(), depthBuffer->getDepthFormat(), imageFormat);
	framebuffer->create(deviceManager.getDevice(), imageViews, msaa->getImageView(), depthBuffer->getImageView(),
		renderpass->getRenderPass(), imageExtent);

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
			return VK_PRESENT_MODE_IMMEDIATE_KHR;
	}
	return VK_PRESENT_MODE_IMMEDIATE_KHR;
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
	vkGetSwapchainImagesKHR(deviceManager.getDevice(), swapChain, &imageCount, nullptr);
	images.resize(imageCount);
	vkGetSwapchainImagesKHR(deviceManager.getDevice(), swapChain, &imageCount, images.data());

}
void SwapchainManager::createImageViews()
{
	imageViews.resize(images.size());

	for (size_t i = 0; i < imageViews.size(); i++) {
		imageViews[i] = createImageView(images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, deviceManager.getDevice());
	}


}
