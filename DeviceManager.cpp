#include "DeviceManager.hpp"

DeviceManager::DeviceManager(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& extensions)
{
	pickPhysicalDevice(instance, surface);
	createLogicalDevice(extensions);
	msaaSamples = getMaxUseableSampleCount();
	minUniformBufferOffset = phyDeviceProps.limits.minUniformBufferOffsetAlignment;
}

void DeviceManager::cleanup()
{
	vkDestroyDevice(device, nullptr);
}

const VkPhysicalDevice& DeviceManager::getPhysicalDevice()
{
	return physicalDevice;
}

const VkPhysicalDeviceProperties& DeviceManager::getPhysicalDeviceProperties()
{
	return phyDeviceProps;
}

const QueueFamilyIndices& DeviceManager::getIndices()
{
	return indices;
}

const VkDevice& DeviceManager::getDevice()
{
	return device;
}

const std::vector<const char*>& DeviceManager::getDeviceExtensions()
{
	return deviceExtensions;
}

VkSampleCountFlagBits DeviceManager::getSampleCount()
{
	return msaaSamples;
}

const VkQueue& DeviceManager::getPresentQueue()
{
	return presentQueue;
}

const VkQueue& DeviceManager::getGraphicsQueue()
{
	return graphicsQueue;
}

void DeviceManager::pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
{
	uint32_t devicesCount = 0;
	vkEnumeratePhysicalDevices(instance, &devicesCount, nullptr);
	if (devicesCount == 0)
	{
		throw std::runtime_error("failed to find GPU");
	}

	std::vector<VkPhysicalDevice> devices(devicesCount);
	vkEnumeratePhysicalDevices(instance, &devicesCount, devices.data());

	for (const auto& device : devices)
	{
		if (isPhysicalDeviceSuitable(device, surface))
		{
			physicalDevice = device;
			msaaSamples = getMaxUseableSampleCount();
			break;
		}
	}

	vkGetPhysicalDeviceProperties(physicalDevice, &phyDeviceProps);

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("no suitable GPU");
	}
}

bool DeviceManager::isPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	indices = findQueueFamilies(device, surface);
	bool extensionsSupported = checkPhysicalDeviceExtensionSupport(device);

	bool isSwapChainSuitable = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainDetails = querySwapChainSupport(device, surface);
		isSwapChainSuitable = !swapChainDetails.formats.empty() && !swapChainDetails.presents.empty();
	}

	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	bool isDedicated = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

	return indices.isComplete() && extensionsSupported && isSwapChainSuitable && isDedicated;
}

void DeviceManager::createLogicalDevice(const std::vector<const char*>& extensions)
{
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};


	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
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

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledLayerNames = extensions.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

VkSampleCountFlagBits DeviceManager::getMaxUseableSampleCount()
{
	VkSampleCountFlags counts = phyDeviceProps.limits.framebufferColorSampleCounts &
		phyDeviceProps.limits.framebufferDepthSampleCounts;
	if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
	if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
	if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
	if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
	if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
	if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

	return VK_SAMPLE_COUNT_1_BIT;
}

bool DeviceManager::checkPhysicalDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}
