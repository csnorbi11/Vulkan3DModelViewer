#pragma once
#include "RendererCommon.h"


class RenderPass {
public:
	RenderPass();
	~RenderPass();

	RenderPass(const VkDevice& device,const VkFormat& swapchainImageFormat,
	const VkSampleCountFlagBits& sampleCount, const VkFormat& depthBufferFormat);

	void create(const VkFormat& swapchainImageFormat, const VkSampleCountFlagBits& sampleCount, const VkFormat& depthBufferFormat, const VkDevice& device);
	void cleanup();

	const VkRenderPass& getRenderPass();

private:
	VkRenderPass renderpass;

	VkDevice device;
};