#include "VulkanRenderer.hpp"

VulkanRenderer::VulkanRenderer(GLFWwindow* window, int& windowWidth, int& windowHeight,
                               const Camera& camera, const std::string configPath)
	:
	framebufferResized(false),
	window(window),
	windowWidth(windowWidth),
	windowHeight(windowHeight),
	currentFrame(0),
	MAX_FRAMES_IN_FLIGHT(2)
{
	readConfig(configPath);

	validationLayers = std::make_unique<ValidationLayers>();
	createInstance();
	createSurface(window);
	validationLayers->setupDebugMessenger(instance);
	deviceManager = std::make_unique<DeviceManager>(instance, surface, validationLayers->getValidationLayers());

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	swapchainManager = std::make_unique<SwapchainManager>(*deviceManager, surface, width, height);

	uniformBuffer = std::make_unique<UniformBuffer>(deviceManager->getDevice(),
	                                                deviceManager->getPhysicalDevice(), MAX_FRAMES_IN_FLIGHT,
	                                                swapchainManager->getImageExtent(),
	                                                deviceManager->getPhysicalDeviceProperties(),
	                                                models, lightSources, camera);

	graphicsPipelines.push_back(GraphicsPipeline(*deviceManager, swapchainManager->getImageExtent(),
	                                             swapchainManager->getImageFormat(),
	                                             swapchainManager->getRenderPass().getRenderPass(),
	                                             MAX_FRAMES_IN_FLIGHT, *uniformBuffer, "modelShader", "shadervert.spv",
	                                             "shader.fragrag.spv"));

	graphicsPipelines.push_back(GraphicsPipeline(*deviceManager, swapchainManager->getImageExtent(),
	                                             swapchainManager->getImageFormat(),
	                                             swapchainManager->getRenderPass().getRenderPass(),
	                                             MAX_FRAMES_IN_FLIGHT, *uniformBuffer, "lightShader",
	                                             "lightblubvert.spv", "lightblub.fragrag.spv"));

	commandbuffer = std::make_unique<CommandBuffer>(deviceManager->getDevice(), deviceManager->getIndices(),
	                                                swapchainManager->getRenderPass().getRenderPass(),
	                                                swapchainManager->getFramebuffer().getSwapchainFramebuffers(),
	                                                swapchainManager->getImageExtent(), graphicsPipelines,
	                                                graphicsPipelines[0].getLayout(),
	                                                MAX_FRAMES_IN_FLIGHT, uniformBuffer->getDynamicAlignment());

	syncObjects = std::make_unique<SyncObjects>(deviceManager->getDevice(), MAX_FRAMES_IN_FLIGHT);

	lightSources.push_back(LightSource(*deviceManager, commandbuffer->getCommandPool(), "light"));
	uniformBuffer->calculateDynamicBuffer();
	uniformBuffer->recreateDynamicBuffer();
	for (auto& lightSource : lightSources)
	{
		uniformBuffer->createDescriptorSets(lightSource, MAX_FRAMES_IN_FLIGHT);
	}
}

VulkanRenderer::~VulkanRenderer()
{
	deleteAllModels();

	swapchainManager->cleanup();

	syncObjects->cleanup(deviceManager->getDevice());
	commandbuffer->cleanup(deviceManager->getDevice());
	for (auto& graphicsPipeline : graphicsPipelines)
		graphicsPipeline.cleanup(deviceManager->getDevice());
	uniformBuffer->cleanup();

	deviceManager->cleanup();
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(instance, validationLayers->getDebugMessenger(), nullptr);
	}
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void VulkanRenderer::createInstance()
{
	if (enableValidationLayers && !validationLayers->checkValidationLayerSupport())
	{
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
	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers->getValidationLayers().size());
		createInfo.ppEnabledLayerNames = validationLayers->getValidationLayers().data();

		validationLayers->populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}


	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance");
	}
}

void VulkanRenderer::createSurface(GLFWwindow* window)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("filed to create surface");
	}
}

void VulkanRenderer::recreateSwapchain()
{
	glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
	while (windowWidth == 0 || windowHeight == 0)
	{
		glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(deviceManager->getDevice());

	swapchainManager->cleanup();

	swapchainManager->recreate(static_cast<uint32_t>(windowWidth), static_cast<uint32_t>(windowHeight));
	commandbuffer->update(*swapchainManager);
}

void VulkanRenderer::readConfig(const std::string configPath)
{
	std::ifstream inputFile(configPath);

	std::string line;

	while (std::getline(inputFile, line))
	{
		//std::cout << line << std::endl;
	}

	inputFile.close();
}

void VulkanRenderer::drawFrame()
{
	vkWaitForFences(deviceManager->getDevice(), 1, &(syncObjects->inFlightFences[currentFrame]),
	                VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(deviceManager->getDevice(), swapchainManager->getSwapchain(), UINT64_MAX,
	                                        syncObjects->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE,
	                                        &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapchain();
		return;
	}
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("faileed to acquire image from swapchain");
	}

	vkResetFences(deviceManager->getDevice(), 1, &syncObjects->inFlightFences[currentFrame]);

	vkResetCommandBuffer(commandbuffer->getCommandbuffers()[currentFrame], 0);
	commandbuffer->recordCommandBuffer(currentFrame, imageIndex, models, lightSources);


	uniformBuffer->updateDynamic(currentFrame);
	uniformBuffer->updateStatic(currentFrame);


	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {syncObjects->imageAvailableSemaphores[currentFrame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandbuffer->getCommandbuffers()[currentFrame];

	VkSemaphore signalSemaphores[] = {syncObjects->renderFinishedSemaphores[currentFrame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(deviceManager->getGraphicsQueue(), 1, &submitInfo, syncObjects->inFlightFences[currentFrame]) !=
		VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {swapchainManager->getSwapchain()};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(deviceManager->getPresentQueue(), &presentInfo);
	if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR || framebufferResized)
	{
		framebufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS)
	{
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
	for (auto& lightSource : lightSources)
		uniformBuffer->createDescriptorSets(lightSource, MAX_FRAMES_IN_FLIGHT);
}

template <typename T>
void VulkanRenderer::deleteObject(T& object)
{
	if (typeid(object) == typeid(Model))
	{
		object->cleanup(deviceManager->getDevice());
		for (auto it = models.begin(); it != models.end(); ++it)
		{
			if (&(*it) == object)
			{
				models.erase(it);
				return;
			}
		}
	}
	else if (typeid(object) == typeid(LightSource))
	{
		object->cleanup(deviceManager->getDevice());
		for (auto it = lightSources.begin(); it != lightSources.end(); ++it)
		{
			if (&(*it) == object)
			{
				lightSources.erase(it);
				return;
			}
		}
	}
}

void VulkanRenderer::deleteAllModels()
{
	for (auto& model : models)
	{
		model.cleanup(deviceManager->getDevice());
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
