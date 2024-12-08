#include "RendererCommon.hpp"
#include <vulkan/vulkan.h>


#include <iostream>
#include <vector>


class VulkanRenderer {
public:
	VulkanRenderer(GLFWwindow* window);
	~VulkanRenderer();

private:
	VkInstance instance;

#if NDEBUG
	bool enableValidationLayers = false;
#else
	bool enableValidationLayers = true;
#endif


	VkDebugUtilsMessengerEXT debugMessenger;
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

private:
	void createInstance();

	void setupDebugMessenger();
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

};


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessneger,
	const VkAllocationCallbacks* pAllocator);