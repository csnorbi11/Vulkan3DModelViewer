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
		const int MAX_FRAMES_IN_FLIGHT, UniformBuffer uniformBuffer,
		const std::string name, const std::string& vertShader,const std::string& fragShader);

	void cleanup(VkDevice device);

	const VkPipeline& getPipeline();
	const VkPipelineLayout& getLayout();
	const std::string& getName();

private:
	VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code);

	const std::string name;

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
};