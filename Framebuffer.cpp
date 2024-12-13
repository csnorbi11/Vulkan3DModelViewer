#include "Framebuffer.hpp"

Framebuffer::Framebuffer()
{
}
Framebuffer::~Framebuffer()
{
}

Framebuffer::Framebuffer(const std::vector<VkImageView>& swapchainImageViews,
	const VkImageView& msaaImageView, const VkImageView depthImageView,
	RenderPass& renderpass, VkExtent2D swapchainExtent,
	const VkDevice device)
	:
	device(device)
{
	swapchainFramebuffers.resize(swapchainImageViews.size());

	for (size_t i = 0; i < swapchainFramebuffers.size(); i++) {
		std::array<VkImageView, 3> attachments = {
			msaaImageView,
			depthImageView,
			swapchainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass.getRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(this->device, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer");
		}

	}
}



void Framebuffer::cleanup()
{
	for (auto framebuffer : swapchainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
}
