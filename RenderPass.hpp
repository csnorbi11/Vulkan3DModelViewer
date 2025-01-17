#pragma once
#include "RendererCommon.h"


class RenderPass
{
public:
	RenderPass() = default;
	~RenderPass() = default;

	RenderPass(VkDevice device, VkFormat swapchainImageFormat,
	           VkSampleCountFlagBits sampleCount, VkFormat depthBufferFormat);

	void create(VkDevice device, VkSampleCountFlagBits sampleCount,
	            VkFormat depthBufferFormat, VkFormat swapchainImageFormat);
	void cleanup(VkDevice device);

	const VkRenderPass& getRenderPass();

private:
	VkRenderPass renderpass;
};
