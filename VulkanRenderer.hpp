#pragma once
#include "RendererCommon.h"
#include "ValidationLayers.hpp"
#include "SwapchainManager.hpp"
#include "DeviceManager.hpp"
#include "Framebuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "CommandBuffer.hpp"
#include "SyncObjects.hpp"



class VulkanRenderer {
public:
	VulkanRenderer(GLFWwindow* window);
	~VulkanRenderer();

	void drawFrame();

	void wait();

	bool framebufferResized;

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
	std::shared_ptr<DepthBuffer> depthBuffer;
	std::shared_ptr<Msaa> msaa;

	std::unique_ptr<GraphicsPipeline> graphicsPipeline;
	std::unique_ptr<Framebuffer> frameBuffer;

	std::unique_ptr<CommandBuffer> commandbuffer;
	
	std::unique_ptr<SyncObjects> syncObjects;
	uint32_t currentFrame;
	const int MAX_FRAMES_IN_FLIGHT;
};
