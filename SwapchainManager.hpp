#pragma once
#include "RendererCommon.h"
#include "DepthBuffer.hpp"
#include "Msaa.hpp"
#include "Framebuffer.hpp"

class SwapchainManager {
public:
	SwapchainManager();
	~SwapchainManager();

	SwapchainManager(const VkPhysicalDevice& phyDevice, const VkDevice& device,
		const std::vector<const char*>& deviceExtensions, const QueueFamilyIndices indices,
		VkSurfaceKHR& surface, uint32_t frameBufferWidth, uint32_t frameBufferHeight,
		VkSampleCountFlagBits sampleCount);
	void create();
	void cleanup();
	void recreate(uint32_t frameBufferWidth, uint32_t frameBufferHeight);

	const VkFormat& getImageFormat();
	const VkExtent2D getImageExtent();
	const std::vector<VkImageView> getImageViews();
	const DepthBuffer& getDepthBuffer();
	const Msaa& getMsaa();
	const VkSwapchainKHR& getSwapchain();


private:
	
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaiableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avaiablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImages(uint32_t& imageCount, VkSurfaceFormatKHR& surfaceFormat);
	void createImageViews();

	VkSwapchainKHR swapChain;


	std::unique_ptr<DepthBuffer> depthBuffer;
	std::unique_ptr<Msaa> msaa;

	std::vector<VkImage> images;
	VkFormat imageFormat;
	VkExtent2D imageExtent;
	std::vector<VkImageView> imageViews;
	uint32_t imagesCount;


	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkSampleCountFlagBits sampleCount;
	std::vector<const char*> extensions;
	QueueFamilyIndices indices;
	uint32_t framebufferWidth, framebufferHeight;
};