#pragma once
#include "RendererCommon.h"
#include "RenderPass.hpp"
#include "UniformBuffer.hpp"

class GraphicsPipeline {
public:
	GraphicsPipeline() = default;
	~GraphicsPipeline();

	GraphicsPipeline(const VkDevice& device, const VkExtent2D& swapchainExtent,
		VkSampleCountFlagBits sampleCount, const VkFormat& swapchainImageFormat,
		const VkPhysicalDevice& physicalDevice, const VkRenderPass& renderpass,
		const int MAX_FRAMES_IN_FLIGHT, VkPhysicalDeviceProperties properties,
		UniformBuffer& uniBuffer);

	void cleanup();

	const VkPipeline& getPipeline();
	const VkPipelineLayout& getLayout();

private:
	VkShaderModule createShaderModule(const std::vector<char>& code);

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

	UniformBuffer& uniformBuffer;

	VkDevice device;
};