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
#include "Camera.hpp"

class VulkanRenderer {
public:
	VulkanRenderer() = default;
	~VulkanRenderer();

	VulkanRenderer(GLFWwindow* window, int& windowWidth, int& windowHeight,
		const Camera& camera);

	void drawFrame();

	void wait();
	void recieveModel(const Model& model);
	void deleteModel(Model& model);
	void deleteAllModels();

	bool framebufferResized;

	DeviceManager& getDeviceManager();
	CommandBuffer& getCommandBuffer();
	VkInstance& getInstance();
	UniformBuffer& getUniformBuffer();
	SwapchainManager& getSwapchainManager();

	std::vector<Model> models;

private:
	void createInstance();
	void createSurface(GLFWwindow* window);

	void recreateSwapchain();


	GLFWwindow* window;
	int& windowWidth;
	int& windowHeight;

	VkInstance instance;
	VkSurfaceKHR surface;

	std::unique_ptr<ValidationLayers> validationLayers;
	std::unique_ptr<DeviceManager> deviceManager;
	std::unique_ptr<SwapchainManager> swapchainManager;
	std::unique_ptr<UniformBuffer> uniformBuffer;
	std::unique_ptr<GraphicsPipeline> graphicsPipeline;
	std::unique_ptr<CommandBuffer> commandbuffer;

	std::unique_ptr<SyncObjects> syncObjects;
	uint32_t currentFrame;
	const int MAX_FRAMES_IN_FLIGHT;
};
