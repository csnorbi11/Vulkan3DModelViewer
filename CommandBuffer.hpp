#pragma once
#include "RendererCommon.h"


class CommandBuffer {
public:
	CommandBuffer();
	~CommandBuffer();

	CommandBuffer(const VkDevice& device, const QueueFamilyIndices& indices,
		const VkRenderPass& renderpass, const std::vector<VkFramebuffer>& swapchainFramebuffers,
		const VkExtent2D& swapchainExtent, const VkPipeline& graphicsPipeline,
		const VkPipelineLayout& pipelineLayout, const std::vector<VkDescriptorSet>& descriptorSets,
		const int MAX_FRAMES_IN_FLIGHT);

	void cleanup();

	void recordCommandBuffer(uint32_t currentFrame, uint32_t imageIndex,
		const VkBuffer& vertexBuffer, const VkBuffer& indexBuffer);
	void setClearColor(VkClearValue newClearValue);
	void updateFramebuffer(const std::vector<VkFramebuffer>& framebuffer);

	std::vector<VkCommandBuffer>& getCommandbuffers();
	const VkCommandPool& getCommandPool();
private:



	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector <VkClearValue> clearValues;

	VkDevice device;
	VkRenderPass renderpass;
	std::vector<VkFramebuffer> swapchainFramebuffers;
	VkExtent2D swapchainExtent;
	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkDescriptorSet> descriptorSets;
	
};