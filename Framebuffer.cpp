#include "Framebuffer.hpp"

Framebuffer::Framebuffer(VkDevice device, const std::vector<VkImageView>& swapchainImageViews,
                         VkImageView msaaImageView, VkImageView depthImageView,
                         VkRenderPass renderpass, VkExtent2D swapchainExtent)
{
	create(device, swapchainImageViews, msaaImageView, depthImageView, renderpass, swapchainExtent);
}


void Framebuffer::create(VkDevice device, const std::vector<VkImageView>& swapchainImageViews,
                         VkImageView msaaImageView,
                         VkImageView depthImageView, VkRenderPass renderpass,
                         VkExtent2D swapchainExtent)
{
	swapchainFramebuffers.resize(swapchainImageViews.size());

	for (size_t i = 0; i < swapchainFramebuffers.size(); i++)
	{
		std::array<VkImageView, 3> attachments = {
			msaaImageView,
			depthImageView,
			swapchainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderpass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapchainExtent.width;
		framebufferInfo.height = swapchainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create framebuffer");
		}
	}
}


void Framebuffer::cleanup(VkDevice device)
{
	for (size_t i = 0; i < swapchainFramebuffers.size(); i++)
	{
		vkDestroyFramebuffer(device, swapchainFramebuffers[i], nullptr);
	}
}

const std::vector<VkFramebuffer>& Framebuffer::getSwapchainFramebuffers()
{
	return swapchainFramebuffers;
}
