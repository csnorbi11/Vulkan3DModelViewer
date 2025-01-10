#pragma once
#include "RendererCommon.h"
#include "RenderPass.hpp"
#include "UniformBuffer.hpp"

class GraphicsPipeline {
public:
	GraphicsPipeline() = default;
	~GraphicsPipeline() = default;

	GraphicsPipeline(VkDevice device, VkExtent2D swapchainExtent,
		VkSampleCountFlagBits sampleCount, VkFormat swapchainImageFormat,
		VkPhysicalDevice physicalDevice, VkRenderPass renderpass,
		const int MAX_FRAMES_IN_FLIGHT, VkPhysicalDeviceProperties properties,
		UniformBuffer uniformBuffer);

	void cleanup(VkDevice device);

	const VkPipeline& getPipeline();
	const VkPipelineLayout& getLayout();

private:
	VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
};