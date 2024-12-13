#pragma once
#include "RendererCommon.h"
#include "RenderPass.hpp"

class Framebuffer {
public:
	Framebuffer();
	~Framebuffer();

	Framebuffer(const std::vector<VkImageView>& swapchainImageViews,
		const VkImageView& msaaImageView, const VkImageView depthImageView,
		const VkRenderPass& renderpass, const VkExtent2D& swapchainExtent,
		const VkDevice device);


	void create(const std::vector<VkImageView>& swapchainImageViews, const VkImageView& msaaImageView,
		const VkImageView depthImageView, const VkRenderPass& renderpass,
		const VkExtent2D& swapchainExtent);
	void cleanup();

	const std::vector<VkFramebuffer>& getSwapchainFramebuffers();


private:

	std::vector<VkFramebuffer> swapchainFramebuffers;

	VkDevice device;

};