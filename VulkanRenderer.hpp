#pragma once
#include "RendererCommon.h"
#include "ValidationLayers.hpp"
#include "SwapchainManager.hpp"
#include "DeviceManager.hpp"
#include "Framebuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "CommandBuffer.hpp"




class VulkanRenderer {
public:
	VulkanRenderer(GLFWwindow* window);
	~VulkanRenderer();

private:
	void createInstance();
	void createSurface(GLFWwindow* window);

	void recreateSwapchain();

	GLFWwindow* window;

	VkInstance instance;
	VkSurfaceKHR surface;

	std::unique_ptr<ValidationLayers> validationLayers;
	std::unique_ptr<DeviceManager> deviceManager;

	std::unique_ptr<SwapchainManager> swapchainManager;
	DepthBuffer depthBuffer;
	Msaa msaa;

	std::unique_ptr<GraphicsPipeline> graphicsPipeline;
	std::unique_ptr<Framebuffer> frameBuffer;

	std::unique_ptr<CommandBuffer> commandbuffer;
	
};
