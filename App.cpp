#include "App.hpp"

App::App()
	:
	glfwHandler(),
	renderer(glfwHandler.window.get()),
	modelLoader(renderer.getDeviceManager().getDevice(),
		renderer.getDeviceManager().getPhysicalDevice(),
		renderer.getCommandBuffer().getCommandPool(), 
		renderer.getDeviceManager().getGraphicsQueue(),
		renderer.getDeviceManager().getPhysicalDeviceProperties())
{
	renderer.recieveModel(modelLoader.loadModel("backpack.obj"));
	


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
	std::cout << "-------------------------------" << std::endl;
	std::cout << "----------loop starts----------" << std::endl;
	std::cout << "-------------------------------" << std::endl;
	bool a = true;
	bool b = true;

	while (!glfwWindowShouldClose(glfwHandler.window.get())) {
		glfwPollEvents();
		
		if (glfwGetTime() > 5.0 &&a)
		{
			renderer.recieveModel(modelLoader.loadModel("viking_room.obj", true));
			a = false;
		}

		
		
		
		renderer.drawFrame();


	}
	renderer.wait();
	std::cout << "-------------------------------" << std::endl;
	std::cout << "----------loop ends----------" << std::endl;
	std::cout << "-------------------------------" << std::endl;
}

void App::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	app->renderer.framebufferResized = true;
}


