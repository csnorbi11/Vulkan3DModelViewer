#include "App.hpp"

App::App()
	:
	glfwHandler(),
	renderer(glfwHandler.window.get())
{
	glfwSetWindowUserPointer(glfwHandler.window.get(), this);
	glfwSetWindowSizeCallback(glfwHandler.window.get(), framebufferResizeCallback);
}

App::~App()
{
	
}

void App::run()
{
	loop();
}


void App::loop()
{
	while (!glfwWindowShouldClose(glfwHandler.window.get())) {
		glfwPollEvents();
		renderer.drawFrame();
	}
}

void App::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<VulkanRenderer*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}


