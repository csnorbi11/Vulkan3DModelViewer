#pragma once
#include "RendererCommon.h"
#include "RenderPass.hpp"
#include "UniformBuffer.hpp"

class GraphicsPipeline {
public:
	GraphicsPipeline();
	~GraphicsPipeline();

	GraphicsPipeline(const VkDevice& device, const VkExtent2D& swapchainExtent,
		VkSampleCountFlagBits sampleCount, const VkFormat& swapchainImageFormat,
		const VkFormat& depthBufferFormat, const VkPhysicalDevice& physicalDevice,
		const int MAX_FRAMES_IN_FLIGHT);

	void cleanup();

	RenderPass& getRenderPass();
	const VkPipeline& getPipeline();
	const VkPipelineLayout& getLayout();
	UniformBuffer& getUniformBuffer();
	const VkDescriptorSetLayout& getDescriptorSetLayout();
	const std::vector<VkDescriptorSet>& getDescriptorSets();

private:
	VkShaderModule createShaderModule(const std::vector<char>& code);

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	std::unique_ptr<RenderPass> renderpass;
	std::unique_ptr<UniformBuffer> uniformBuffer;

	VkDevice device;
};