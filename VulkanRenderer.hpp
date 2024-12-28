#pragma once
#include "RendererCommon.h"
#include "ValidationLayers.hpp"
#include "SwapchainManager.hpp"
#include "DeviceManager.hpp"
#include "Framebuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "CommandBuffer.hpp"
#include "SyncObjects.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "UniformBuffer.hpp"
#include "Model.hpp"

class VulkanRenderer {
public:
	VulkanRenderer(GLFWwindow* window);
	~VulkanRenderer();

	void drawFrame();

	void wait();
	void recieveModel(const Model& model);
	void deleteModel(const Model& model);

	bool framebufferResized;

	DeviceManager& getDeviceManager();
	CommandBuffer& getCommandBuffer();
	

private:
	void createInstance();
	void createSurface(GLFWwindow* window);

	void recreateSwapchain();

	std::vector<Model> models;

	GLFWwindow* window;

	VkInstance instance;
	VkSurfaceKHR surface;

	std::unique_ptr<ValidationLayers> validationLayers;
	std::unique_ptr<DeviceManager> deviceManager;
	std::unique_ptr<SwapchainManager> swapchainManager;
	std::unique_ptr<GraphicsPipeline> graphicsPipeline;
	std::unique_ptr<CommandBuffer> commandbuffer;
	
	std::unique_ptr<SyncObjects> syncObjects;
	uint32_t currentFrame;
	const int MAX_FRAMES_IN_FLIGHT;
};
