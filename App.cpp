#include "App.hpp"

App::App()
	:
	glfwHandler(),
	renderer(glfwHandler.window.get())
{

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
	}
}

