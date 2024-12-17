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
	renderer.wait();
}

void App::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	app->renderer.framebufferResized = true;
}


