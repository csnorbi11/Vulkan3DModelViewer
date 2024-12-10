#pragma once
#include "GlfwHandler.hpp"
#include "VulkanRenderer.hpp"

class App {
public:
	App();
	~App();

	void run();

private:
	void loop();


	GlfwHandler glfwHandler;
	VulkanRenderer renderer;

};