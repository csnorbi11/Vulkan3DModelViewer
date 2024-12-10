#include "RendererCommon.h"
#include "ValidationLayers.hpp"
#include "SwapchainManager.hpp"

#include <vector>




class VulkanRenderer {
public:
	VulkanRenderer(GLFWwindow* window);
	~VulkanRenderer();

private:
	VkInstance instance;
	ValidationLayers validationLayers;

	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice;
	VkPhysicalDeviceProperties phyDeviceProps;
	const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;

	SwapchainManager swapchainManager;


private:
	void createInstance();

	void createSurface(GLFWwindow* window);

	void pickPhysicalDevice();
	bool isPhysicalDeviceSuitable(VkPhysicalDevice device);
	bool checkPhysicalDeviceExtensionSupport(VkPhysicalDevice device);
	

	void createLogicalDevice();



};
