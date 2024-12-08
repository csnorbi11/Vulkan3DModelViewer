#include "RendererCommon.h"

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	{
		QueueFamilyIndices indices;

		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilies.data());

		int i = 0;
		for (const auto& family : queueFamilies) {
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupported = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupported);
			if (presentSupported) {
				indices.presentFamily = i;
			}

			if (indices.isComplete())
				break;
			i++;
		}

		return indices;
	}
}
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilites);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presents.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presents.data());
	}

	return details;
}
