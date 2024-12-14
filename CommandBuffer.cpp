#include "CommandBuffer.hpp"

CommandBuffer::CommandBuffer()
{
}
CommandBuffer::~CommandBuffer()
{
}

CommandBuffer::CommandBuffer(const VkDevice& device, const QueueFamilyIndices& indices,
	const VkRenderPass& renderpass, const std::vector<VkFramebuffer>& swapchainFramebuffers,
	const VkExtent2D& swapchainExtent, const VkPipeline& graphicsPipeline,
	const int MAX_FRAMES_IN_FLIGHT)
	:
	device(device),
	renderpass(renderpass),
	swapchainFramebuffers(swapchainFramebuffers),
	swapchainExtent(swapchainExtent),
	graphicsPipeline(graphicsPipeline)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool");
	}

	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}


	clearValues.resize(MAX_FRAMES_IN_FLIGHT);
	clearValues[0].color = { 0.0,0.0,0.0 };
	clearValues[1].color = { 0.0,0.0,0.0 };
}

void CommandBuffer::cleanup()
{
	vkDestroyCommandPool(device, commandPool, nullptr);


}

void CommandBuffer::recordCommandBuffer(uint32_t currentFrame, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderpass;
	renderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchainExtent;

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchainExtent.width);
	viewport.height = static_cast<float>(swapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchainExtent;
	vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);

	vkCmdDraw(commandBuffers[currentFrame], 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffers[currentFrame]);
	if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void CommandBuffer::setClearColor(VkClearValue newClearValue)
{
	clearValues[0] = newClearValue;
	clearValues[1] = newClearValue;
}

void CommandBuffer::updateFramebuffer(const std::vector<VkFramebuffer>& framebuffer)
{
	swapchainFramebuffers = framebuffer;
}

std::vector<VkCommandBuffer>& CommandBuffer::getCommandbuffers()
{
	return commandBuffers;
}
