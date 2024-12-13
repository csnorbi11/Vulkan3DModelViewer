#include "RendererCommon.h"
#include "RenderPass.hpp"

class GraphicsPipeline {
public:
	GraphicsPipeline();
	~GraphicsPipeline();

	GraphicsPipeline(const VkDevice& device, const VkExtent2D& swapchainExtent,
		VkSampleCountFlagBits sampleCount, const VkFormat& swapchainImageFormat,
		const VkFormat& depthBufferFormat);

	void cleanup();

	RenderPass& getRenderPass();

private:
	VkShaderModule createShaderModule(const std::vector<char>& code);

	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	std::unique_ptr<RenderPass> renderpass;

	VkDevice device;
};