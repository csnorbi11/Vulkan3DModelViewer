#include "VulkanRenderer.hpp"
#include <set>

VulkanRenderer::VulkanRenderer(GLFWwindow* window)
{
	createInstance();
	createSurface(window);
	validationLayers.setupDebugMessenger(instance);
	pickPhysicalDevice();
	createLogicalDevice();
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	swapchain.createSwapChain(physicalDevice, device, surface,
		static_cast<uint32_t>(width),static_cast<uint32_t>(height));
}
VulkanRenderer::~VulkanRenderer()
{


	vkDestroyDevice(device, nullptr);
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, validationLayers.getDebugMessenger(), nullptr);
	}
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void VulkanRenderer::createInstance()
{
	if (enableValidationLayers && !validationLayers.checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan 3D Model Viewer";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;


	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = validationLayers.getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.validationLayers.data();

		validationLayers.populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}


	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance");
	}

}

void VulkanRenderer::createSurface(GLFWwindow* window)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("filed to create surface");
	}
}

void VulkanRenderer::pickPhysicalDevice()
{
	uint32_t devicesCount = 0;
	vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);
	if (devicesCount == 0) {
		throw std::runtime_error("failed to find GPU");
	}

	std::vector<VkPhysicalDevice> devices(devicesCount);
	vkEnumeratePhysicalDevices(instance, &devicesCount, devices.data());

	for (const auto& device : devices) {
		if (isPhysicalDeviceSuitable(device)) {
			physicalDevice = device;


			break;
		}
	}

	vkGetPhysicalDeviceProperties(physicalDevice, &phyDeviceProps);

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("no suitable GPU");
	}

}
bool VulkanRenderer::isPhysicalDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = findQueueFamilies(device, surface);

	bool extensionsSupported = checkPhysicalDeviceExtensionSupport(device);

	bool isSwapChainSuitable = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainDetails = querySwapChainSupport(device, surface);
		isSwapChainSuitable = !swapChainDetails.formats.empty() && !swapChainDetails.presents.empty();
	}


	return indices.isComplete() && extensionsSupported && isSwapChainSuitable;
}
bool VulkanRenderer::checkPhysicalDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}


void VulkanRenderer::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(),
		indices.presentFamily.value() };


	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.sampleRateShading = VK_TRUE;

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

}

