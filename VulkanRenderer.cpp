#include "VulkanRenderer.hpp"

VulkanRenderer::VulkanRenderer(GLFWwindow* window, int& windowWidth, int& windowHeight,
	const Camera& camera)
	:
	window(window),
	MAX_FRAMES_IN_FLIGHT(2),
	currentFrame(0),
	framebufferResized(false),
	windowWidth(windowWidth),
	windowHeight(windowHeight)
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

	uniformBuffer = std::make_unique<UniformBuffer>(deviceManager->getDevice(),
		deviceManager->getPhysicalDevice(), MAX_FRAMES_IN_FLIGHT, swapchainManager->getImageExtent(), deviceManager->getPhysicalDeviceProperties(),
		models, camera);

	graphicsPipeline = std::make_unique<GraphicsPipeline>(deviceManager->getDevice(), swapchainManager->getImageExtent(),
		swapchainManager->getMsaa().getSampleCount(), swapchainManager->getImageFormat(),
		deviceManager->getPhysicalDevice(), swapchainManager->getRenderPass().getRenderPass(),
		MAX_FRAMES_IN_FLIGHT, deviceManager->getPhysicalDeviceProperties(), *uniformBuffer);

	commandbuffer = std::make_unique<CommandBuffer>(deviceManager->getDevice(), deviceManager->getIndices(),
		swapchainManager->getRenderPass().getRenderPass(), swapchainManager->getFramebuffer().getSwapchainFramebuffers(),
		swapchainManager->getImageExtent(), graphicsPipeline->getPipeline(), graphicsPipeline->getLayout(),
		MAX_FRAMES_IN_FLIGHT, uniformBuffer->getDynamicAlignment());

	syncObjects = std::make_unique<SyncObjects>(deviceManager->getDevice(), MAX_FRAMES_IN_FLIGHT);

}
VulkanRenderer::~VulkanRenderer()
{

	deleteAllModels();

	swapchainManager->cleanup();

	syncObjects->cleanup();
	commandbuffer->cleanup();
	graphicsPipeline->cleanup();
	uniformBuffer->cleanup();

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
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	while (windowWidth == 0 || windowHeight == 0) {
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(deviceManager->getDevice());

	swapchainManager->cleanup();

	swapchainManager->recreate(static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight));
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
	commandbuffer->recordCommandBuffer(currentFrame, imageIndex, models);


	uniformBuffer->updateDynamic(currentFrame);
	uniformBuffer->updateStatic(currentFrame);


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

void VulkanRenderer::recieveModel(const Model& model)
{
	models.push_back(model);
	uniformBuffer->calculateDynamicBuffer();
	uniformBuffer->recreateDynamicBuffer();
	for (auto& model : models)
		uniformBuffer->createDescriptorSets(model, MAX_FRAMES_IN_FLIGHT);
}

void VulkanRenderer::deleteModel(Model& model)
{
	model.cleanup();
	for (auto it = models.begin(); it != models.end(); it++)
	{
		if (&(*it) == &model)
		{
			models.erase(it);
			return;
		}
	}
}

void VulkanRenderer::deleteAllModels()
{
	for (auto& model : models)
	{
		model.cleanup();
	}
	models.clear();
}

DeviceManager& VulkanRenderer::getDeviceManager()
{
	return *deviceManager;
}
CommandBuffer& VulkanRenderer::getCommandBuffer()
{
	return *commandbuffer;
}

VkInstance& VulkanRenderer::getInstance()
{
	return instance;
}

UniformBuffer& VulkanRenderer::getUniformBuffer()
{
	return *uniformBuffer;
}

SwapchainManager& VulkanRenderer::getSwapchainManager()
{
	return *swapchainManager;
}
