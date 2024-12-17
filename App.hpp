#pragma once
#include "GlfwHandler.hpp"
#include "VulkanRenderer.hpp"
#include "ModelLoader.hpp"

class App {
public:
	App();
	~App();

	void run();

private:
	void loop();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	GlfwHandler glfwHandler;
	VulkanRenderer renderer;

};