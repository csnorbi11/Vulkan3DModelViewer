#pragma once
#include "RendererCommon.h"
#include "ValidationLayers.hpp"

class DeviceManager {
public:
	DeviceManager();
	~DeviceManager();


public:
	void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
	bool isPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
	bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice device);

	void createLogicalDevice(ValidationLayers valLayers);

public:
	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties phyDeviceProps;
	QueueFamilyIndices indices;

	const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDevice device;


	VkQueue graphicsQueue;
	VkQueue presentQueue;

};