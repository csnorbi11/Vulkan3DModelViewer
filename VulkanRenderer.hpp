#pragma once
#include "RendererCommon.h"
#include "ValidationLayers.hpp"
#include "SwapchainManager.hpp"
#include "DeviceManager.hpp"

#include <vector>




class VulkanRenderer {
public:
	VulkanRenderer(GLFWwindow* window);
	~VulkanRenderer();

private:
	VkInstance instance;
	ValidationLayers validationLayers;

	VkSurfaceKHR surface;

	DeviceManager deviceManager;

	SwapchainManager swapchainManager;


private:
	void createInstance();

	void createSurface(GLFWwindow* window);

	void pickPhysicalDevice();
	bool isPhysicalDeviceSuitable(VkPhysicalDevice device);
	bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice device);
	

	void createLogicalDevice();



};
