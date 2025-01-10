#pragma once
#include "RendererCommon.h"
#include "RenderPass.hpp"

class Framebuffer {
public:
	Framebuffer() = default;
	~Framebuffer() = default;

	Framebuffer(VkDevice device,const std::vector<VkImageView>& swapchainImageViews,
		VkImageView msaaImageView, VkImageView depthImageView,
		VkRenderPass renderpass, VkExtent2D swapchainExtent);


	void create(VkDevice device, const std::vector<VkImageView>& swapchainImageViews, VkImageView msaaImageView,
		VkImageView depthImageView, VkRenderPass renderpass,
		VkExtent2D swapchainExtent);
	void cleanup(VkDevice device);

	const std::vector<VkFramebuffer>& getSwapchainFramebuffers();


private:

	std::vector<VkFramebuffer> swapchainFramebuffers;

	;

};