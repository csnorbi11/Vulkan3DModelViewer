#pragma once
#include "RendererCommon.h"


class CommandBuffer {
public:
	CommandBuffer();
	~CommandBuffer();

	CommandBuffer(const VkDevice& device, const QueueFamilyIndices& indices,
		const VkRenderPass& renderpass, const std::vector<VkFramebuffer>& swapchainFramebuffers,
		const VkExtent2D& swapchainExtent, const VkPipeline& graphicsPipeline,
		const int MAX_FRAMES_IN_FLIGHT);

	void cleanup();

	void recordCommandBuffer(uint32_t currentFrame, uint32_t imageIndex);
	void setClearColor(VkClearValue newClearValue);

	std::vector<VkCommandBuffer>& getCommandbuffers();
private:



	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector <VkClearValue> clearValues;

	VkDevice device;
	VkRenderPass renderpass;
	std::vector<VkFramebuffer> swapchainFramebuffers;
	VkExtent2D swapchainExtent;
	VkPipeline graphicsPipeline;
};