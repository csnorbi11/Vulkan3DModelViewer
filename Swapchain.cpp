#include "Swapchain.hpp"

Swapchain::Swapchain()
{
}
Swapchain::~Swapchain()
{
	cleanupSwapChain();
}

VkSwapchainKHR Swapchain::get()
{
	return swapChain;
}
void Swapchain::createSwapChain(VkPhysicalDevice phyDevice, VkDevice device, VkSurfaceKHR surface,
	uint32_t frameBufferWidth, uint32_t frameBufferHeight)
{
	this->frameBufferWidth = frameBufferWidth;
	this->frameBufferHeight = frameBufferHeight;

	SwapChainSupportDetails swapChainDetails = querySwapChainSupport(phyDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainDetails.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainDetails.presents);
	VkExtent2D extent = chooseSwapExtent(swapChainDetails.capabilites);

	VkSwapchainCreateInfoKHR swapCreateInfo{};


	if (vkCreateSwapchainKHR(device, &swapCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swapchain");
	}
}
void Swapchain::recreateSwapChain(VkPhysicalDevice phyDevice, VkDevice device, VkSurfaceKHR surface,
	uint32_t frameBufferWidth, uint32_t frameBufferHeight)
{
	this->frameBufferWidth = frameBufferWidth;
	this->frameBufferHeight = frameBufferHeight;

	vkDeviceWaitIdle(device);

	cleanupSwapChain();

	createSwapChain(phyDevice, device, surface, frameBufferWidth, frameBufferHeight);
}
void Swapchain::cleanupSwapChain()
{
}
VkSurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}
VkPresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avaiablePresentModes)
{
	return VkPresentModeKHR();
}
VkExtent2D Swapchain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		VkExtent2D actualExtent = { frameBufferWidth,frameBufferHeight };
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}