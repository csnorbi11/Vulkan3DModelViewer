#pragma once
#include "RendererCommon.h"


class CommandBuffer {
public:
	CommandBuffer();
	~CommandBuffer();

	CommandBuffer(const VkDevice& device, const QueueFamilyIndices& indices,
		const VkRenderPass& renderpass, const std::vector<VkFramebuffer>& swapchainFramebuffers,
		const VkExtent2D& swapchainExtent, const VkPipeline& graphicsPipeline);

	void cleanup();

	void recordCommandBuffer(uint32_t imageIndex);
	void setClearColor(VkClearValue newClearValue);
private:



	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	VkClearValue clearValue;

	VkDevice device;
	VkRenderPass renderpass;
	std::vector<VkFramebuffer> swapchainFramebuffers;
	VkExtent2D swapchainExtent;
	VkPipeline graphicsPipeline;
};