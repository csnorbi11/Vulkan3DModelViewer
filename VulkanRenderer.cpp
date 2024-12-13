#include "VulkanRenderer.hpp"

VulkanRenderer::VulkanRenderer(GLFWwindow* window)
	:
	window(window)
{
	validationLayers = std::make_unique<ValidationLayers>();
	createInstance();
	createSurface(window);
	validationLayers->setupDebugMessenger(instance);
	deviceManager = std::make_unique<DeviceManager>(instance, surface, validationLayers->getValidationLayers());

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	swapchainManager = std::make_unique<SwapchainManager>(deviceManager->getPhysicalDevice(), deviceManager->getDevice(),
		deviceManager->getDeviceExtensions(), deviceManager->getIndices(), surface, width, height,
		deviceManager->getSampleCount());
	depthBuffer = swapchainManager->getDepthBuffer();
	msaa = swapchainManager->getMsaa();

	graphicsPipeline = std::make_unique<GraphicsPipeline>(deviceManager->getDevice(), swapchainManager->getImageExtent(),
		msaa.getSampleCount(), swapchainManager->getImageFormat(), depthBuffer.getDepthFormat());

	frameBuffer = std::make_unique<Framebuffer>(swapchainManager->getImageViews(),
		msaa.getImageView(), depthBuffer.getImageView(), graphicsPipeline->getRenderPass().getRenderPass(),
		swapchainManager->getImageExtent(), deviceManager->getDevice());

	commandbuffer = std::make_unique<CommandBuffer>(deviceManager->getDevice(), deviceManager->getIndices(),
		graphicsPipeline->getRenderPass().getRenderPass(), frameBuffer->getSwapchainFramebuffers(),
		swapchainManager->getImageExtent(), graphicsPipeline->getPipeline());
}
VulkanRenderer::~VulkanRenderer()
{
	commandbuffer->cleanup();
	frameBuffer->cleanup();
	graphicsPipeline->cleanup();
	swapchainManager->cleanup();
	deviceManager->cleanup();
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, validationLayers->getDebugMessenger(), nullptr);
	}
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void VulkanRenderer::createInstance()
{
	if (enableValidationLayers && !validationLayers->checkValidationLayerSupport()) {
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

	auto extensions = validationLayers->getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers->getValidationLayers().size());
		createInfo.ppEnabledLayerNames = validationLayers->getValidationLayers().data();

		validationLayers->populateDebugMessengerCreateInfo(debugCreateInfo);
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

void VulkanRenderer::recreateSwapchain()
{
	vkDeviceWaitIdle(deviceManager->getDevice());


	msaa.cleanup();
	depthBuffer.cleanup();
	swapchainManager->cleanup();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	swapchainManager->recreate(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	msaa.create();
	depthBuffer.create();

}


