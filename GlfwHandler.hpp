#pragma once
#include "RendererCommon.h"

#include <memory>

struct GLFWwindowDeleter {
	void operator()(GLFWwindow* window) {
		if (window) {
			glfwDestroyWindow(window);
		}
	}
};

struct GlfwHandler {
	GlfwHandler();
	~GlfwHandler();
	
	std::unique_ptr<GLFWwindow, GLFWwindowDeleter> window;
	int WIDTH, HEIGHT;


};


