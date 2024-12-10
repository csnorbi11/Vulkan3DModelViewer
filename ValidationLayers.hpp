#pragma once
#include "RendererCommon.h"



class ValidationLayers {
public:
	ValidationLayers();
	~ValidationLayers();

	void setupDebugMessenger(VkInstance instance);
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	const VkDebugUtilsMessengerEXT& getDebugMessenger();
	const std::vector<const char*>& getValidationLayers();

private:
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	VkDebugUtilsMessengerEXT debugMessenger;
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};
};


VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessneger,
	const VkAllocationCallbacks* pAllocator);