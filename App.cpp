#include "App.hpp"

App::App()
	:
	glfwHandler(),
	renderer(glfwHandler.window.get()),
	modelLoader(renderer.getDeviceManager().getDevice(),
		renderer.getDeviceManager().getPhysicalDevice(),
		renderer.getCommandBuffer().getCommandPool(), 
		renderer.getDeviceManager().getGraphicsQueue(),
		renderer.getModels())
{
	modelLoader.loadModel("viking_room.obj");

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
	renderer.wait();
}

void App::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	app->renderer.framebufferResized = true;
}


