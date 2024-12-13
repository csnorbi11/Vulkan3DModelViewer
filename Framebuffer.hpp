#pragma once
#include "RendererCommon.h"
#include "RenderPass.hpp"

class Framebuffer {
public:
	Framebuffer();
	~Framebuffer();

	Framebuffer(const std::vector<VkImageView>& swapchainImageViews,
		const VkImageView& msaaImageView, const VkImageView depthImageView,
		RenderPass& renderpass, VkExtent2D swapchainExtent,
		const VkDevice device);


	void cleanup();


private:

	std::vector<VkFramebuffer> swapchainFramebuffers;

	VkDevice device;

};