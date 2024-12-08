#include "App.hpp"

App::App()
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

