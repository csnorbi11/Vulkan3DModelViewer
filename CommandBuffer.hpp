#pragma once
#include "RendererCommon.h"
#include "SwapchainManager.hpp"
#include "Model.hpp"
#include "LightSource.hpp"
#include "GraphicsPipeline.hpp"

class CommandBuffer
{
public:
	CommandBuffer() = default;
	~CommandBuffer() = default;

	CommandBuffer(VkDevice device, QueueFamilyIndices indices,
	              VkRenderPass renderpass, const std::vector<VkFramebuffer>& swapchainFramebuffers,
	              VkExtent2D swapchainExtent, const std::vector<GraphicsPipeline>& graphicsPipelines,
	              VkPipelineLayout pipelineLayout,
	              int MAX_FRAMES_IN_FLIGHT, uint32_t dynamicAlignment);

	void cleanup(VkDevice device);

	void recordCommandBuffer(uint32_t currentFrame, uint32_t imageIndex,
	                         std::vector<Model>& models, std::vector<LightSource>& lightSources);
	void setClearColor(VkClearValue newClearValue);
	void update(SwapchainManager& swapchainManager);

	std::vector<VkCommandBuffer>& getCommandbuffers();
	VkCommandPool getCommandPool();

private:
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkClearValue> clearValues;


	VkRenderPass renderpass;
	std::vector<VkFramebuffer> swapchainFramebuffers;
	VkExtent2D swapchainExtent;
	std::vector<GraphicsPipeline> graphicsPipelines;
	VkPipelineLayout pipelineLayout;
	uint32_t dynamicAlignment;
};
