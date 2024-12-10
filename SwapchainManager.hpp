#pragma once
#include "RendererCommon.h"
#include "DeviceManager.hpp"


class SwapchainManager {
public:
	SwapchainManager();
	~SwapchainManager();

	SwapchainManager(const VkPhysicalDevice& phyDevice, const VkDevice& device,
		const std::vector<const char*>& deviceExtensions, const QueueFamilyIndices indices,
		VkSurfaceKHR& surface, uint32_t frameBufferWidth, uint32_t frameBufferHeight);
	void cleanup();
	void recreate(uint32_t frameBufferWidth, uint32_t frameBufferHeight);

	VkSwapchainKHR get();

private:
	void create();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaiableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avaiablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);


	VkSwapchainKHR swapChain;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	std::vector<const char*> extensions;
	QueueFamilyIndices indices;
	uint32_t framebufferWidth, framebufferHeight;
};