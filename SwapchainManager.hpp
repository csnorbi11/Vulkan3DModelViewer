#pragma once
#include "RendererCommon.h"
#include "DepthBuffer.hpp"

class SwapchainManager {
public:
	SwapchainManager();
	~SwapchainManager();

	SwapchainManager(const VkPhysicalDevice& phyDevice, const VkDevice& device,
		const std::vector<const char*>& deviceExtensions, const QueueFamilyIndices indices,
		VkSurfaceKHR& surface, uint32_t frameBufferWidth, uint32_t frameBufferHeight);
	void cleanup();
	void recreate(uint32_t frameBufferWidth, uint32_t frameBufferHeight);

	const VkSwapchainKHR& get();
	const VkExtent2D getImageExtent();
 

private:
	void create();
	
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaiableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avaiablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImages(uint32_t& imageCount, VkSurfaceFormatKHR& surfaceFormat);
	void createImageViews();

	VkSwapchainKHR swapChain;
	VkFramebuffer framebuffer;

	std::unique_ptr<DepthBuffer> depthBuffer;

	std::vector<VkImage> images;
	VkFormat imageFormat;
	VkExtent2D imageExtent;
	std::vector<VkImageView> imageViews;


	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	std::vector<const char*> extensions;
	QueueFamilyIndices indices;
	uint32_t framebufferWidth, framebufferHeight;
};