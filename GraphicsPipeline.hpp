#pragma once
#include "RendererCommon.h"
#include "DeviceManager.hpp"
#include "UniformBuffer.hpp"
#include "Shader.hpp"


class GraphicsPipeline
{
public:
	GraphicsPipeline() = default;
	~GraphicsPipeline() = default;

	GraphicsPipeline(DeviceManager& deviceManager, VkExtent2D swapchainExtent,
	                 VkFormat swapchainImageFormat, VkRenderPass renderpass,
	                 int MAX_FRAMES_IN_FLIGHT, UniformBuffer uniformBuffer,
	                 std::string name, const Shader& vertexShader, const Shader& fragmentShader);

	void cleanup(VkDevice device);

	const VkPipeline& getPipeline();
	const VkPipelineLayout& getLayout();
	const std::string& getName();

private:

	const std::string name;

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
};
