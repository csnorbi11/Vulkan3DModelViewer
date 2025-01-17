#pragma once
#include "RendererCommon.h"
#include "DepthBuffer.hpp"
#include "Msaa.hpp"
#include "Framebuffer.hpp"
#include "DeviceManager.hpp"

class SwapchainManager
{
public:
	SwapchainManager() = default;
	~SwapchainManager() = default;

	SwapchainManager(DeviceManager& deviceManager,
	                 VkSurfaceKHR surface, uint32_t frameBufferWidth, uint32_t frameBufferHeight);
	void create();
	void cleanup();
	void recreate(uint32_t frameBufferWidth, uint32_t frameBufferHeight);

	const VkFormat& getImageFormat();
	const VkExtent2D getImageExtent();
	const std::vector<VkImageView> getImageViews();
	const VkSwapchainKHR& getSwapchain();
	Msaa& getMsaa();
	DepthBuffer& getDepthBuffer();
	RenderPass& getRenderPass();
	Framebuffer& getFramebuffer();

private:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaiableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avaiablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImages(uint32_t& imageCount, VkSurfaceFormatKHR& surfaceFormat);
	void createImageViews();

	VkSwapchainKHR swapChain;

	std::unique_ptr<Framebuffer> framebuffer;
	std::unique_ptr<DepthBuffer> depthBuffer;
	std::unique_ptr<Msaa> msaa;
	std::unique_ptr<RenderPass> renderpass;

	std::vector<VkImage> images;
	VkFormat imageFormat;
	VkExtent2D imageExtent;
	std::vector<VkImageView> imageViews;
	uint32_t imagesCount;


	VkSurfaceKHR surface;
	DeviceManager& deviceManager;
	uint32_t framebufferWidth, framebufferHeight;
};
