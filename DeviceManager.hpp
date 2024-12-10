#pragma once
#include "RendererCommon.h"
#include "ValidationLayers.hpp"

class DeviceManager {
public:
	DeviceManager();
	~DeviceManager();

	DeviceManager(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& extensions);
	void cleanup();

	const VkPhysicalDevice& getPhysicalDevice();
	const VkPhysicalDeviceProperties& getPhysicalDeviceProperties();
	const QueueFamilyIndices& getIndices();
	const VkDevice& getDevice();
	const std::vector<const char*>& getDeviceExtensions();

private:
	void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
	bool isPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
	bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice device);

	void createLogicalDevice(const std::vector<const char*>& extensions);

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