#include "VulkanRenderer.hpp"

VulkanRenderer::VulkanRenderer(GLFWwindow* window)
	:
	window(window),
	MAX_FRAMES_IN_FLIGHT(2),
	currentFrame(0),
	framebufferResized(false)
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


	graphicsPipeline = std::make_unique<GraphicsPipeline>(deviceManager->getDevice(), swapchainManager->getImageExtent(),
		swapchainManager->getMsaa().getSampleCount(), swapchainManager->getImageFormat(),
		deviceManager->getPhysicalDevice(), swapchainManager->getRenderPass().getRenderPass(),
		MAX_FRAMES_IN_FLIGHT);
	
	commandbuffer = std::make_unique<CommandBuffer>(deviceManager->getDevice(),deviceManager->getIndices(),
		swapchainManager->getRenderPass().getRenderPass(), swapchainManager->getFramebuffer().getSwapchainFramebuffers(),
		swapchainManager->getImageExtent(),graphicsPipeline->getPipeline(),graphicsPipeline->getLayout(),
		graphicsPipeline->getDescriptorSets(), MAX_FRAMES_IN_FLIGHT);

	syncObjects = std::make_unique<SyncObjects>(deviceManager->getDevice(), MAX_FRAMES_IN_FLIGHT);

	models = std::make_shared<std::vector<Model>>();

}
VulkanRenderer::~VulkanRenderer()
{

	swapchainManager->cleanup();

	
	syncObjects->cleanup();
	commandbuffer->cleanup();
	graphicsPipeline->cleanup();
	
	
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
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(deviceManager->getDevice());

	
	swapchainManager->cleanup();
	

	
	swapchainManager->recreate(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	commandbuffer->update(*swapchainManager);
}

void VulkanRenderer::drawFrame()
{
	vkWaitForFences(deviceManager->getDevice(), 1, &(syncObjects->inFlightFences[currentFrame]),
		VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(deviceManager->getDevice(), swapchainManager->getSwapchain(), UINT64_MAX,
		syncObjects->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("faileed to acquire image from swapchain");
	}

	vkResetFences(deviceManager->getDevice(), 1, &syncObjects->inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandbuffer->getCommandbuffers()[currentFrame], 0);
	commandbuffer->recordCommandBuffer(currentFrame, imageIndex,models->at(0));

	graphicsPipeline->getUniformBuffer().update(currentFrame, currentFrame);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { syncObjects->imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandbuffer->getCommandbuffers()[currentFrame];

	VkSemaphore signalSemaphores[] = { syncObjects->renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(deviceManager->getGraphicsQueue(), 1, &submitInfo, syncObjects->inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapchainManager->getSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(deviceManager->getPresentQueue(), &presentInfo);
	if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanRenderer::wait()
{
	vkDeviceWaitIdle(deviceManager->getDevice());
}

DeviceManager& VulkanRenderer::getDeviceManager()
{
	return *deviceManager;
}
CommandBuffer& VulkanRenderer::getCommandBuffer()
{
	return *commandbuffer;
}

std::shared_ptr<std::vector<Model>> VulkanRenderer::getModels()
{
	return models;
}

