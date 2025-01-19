#pragma once
#include "RendererCommon.h"
#include "ValidationLayers.hpp"
#include "SwapchainManager.hpp"
#include "DeviceManager.hpp"
#include "GraphicsPipeline.hpp"
#include "CommandBuffer.hpp"
#include "SyncObjects.hpp"
#include "UniformBuffer.hpp"
#include "Model.hpp"
#include "Camera.hpp"
#include "LightSource.hpp"

class VulkanRenderer
{
public:
	VulkanRenderer() = default;
	~VulkanRenderer();

	VulkanRenderer(GLFWwindow* window, int& windowWidth, int& windowHeight,
	               const Camera& camera, std::string configPath);

	void drawFrame();

	void wait() const;
	void recieveModel(const Model& model);
	void addLightSource();
	void deleteLightSource(LightSource& lightSource);
	void deleteModel(Model& model);
	void deleteAllModels();

	bool framebufferResized;

	DeviceManager& getDeviceManager();
	CommandBuffer& getCommandBuffer();
	VkInstance& getInstance();
	UniformBuffer& getUniformBuffer();
	SwapchainManager& getSwapchainManager();

	std::vector<Model> models;
	std::vector<LightSource> lightSources;

private:
	void createInstance();
	void createSurface(GLFWwindow* window);

	void recreateSwapchain();

	void readConfig(std::string configPath);

	GLFWwindow* window;
	int& windowWidth;
	int& windowHeight;

	VkInstance instance;
	VkSurfaceKHR surface;

	std::unique_ptr<ValidationLayers> validationLayers;
	std::unique_ptr<DeviceManager> deviceManager;
	std::unique_ptr<SwapchainManager> swapchainManager;
	std::unique_ptr<UniformBuffer> uniformBuffer;
	std::vector<GraphicsPipeline> graphicsPipelines;
	std::unique_ptr<CommandBuffer> commandbuffer;

	std::unique_ptr<SyncObjects> syncObjects;
	uint32_t currentFrame;
	const int MAX_FRAMES_IN_FLIGHT;
};
