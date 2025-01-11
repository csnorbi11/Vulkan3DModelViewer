#pragma once
#include "RendererCommon.h"
#include "RenderPass.hpp"
#include "UniformBuffer.hpp"
#include "DeviceManager.hpp"

class GraphicsPipeline {
public:
	GraphicsPipeline() = default;
	~GraphicsPipeline() = default;

	GraphicsPipeline(DeviceManager& deviceManager, VkExtent2D swapchainExtent,
		 VkFormat swapchainImageFormat, VkRenderPass renderpass,
		const int MAX_FRAMES_IN_FLIGHT, UniformBuffer uniformBuffer);

	void cleanup(VkDevice device);

	const VkPipeline& getPipeline();
	const VkPipelineLayout& getLayout();

private:
	VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
};