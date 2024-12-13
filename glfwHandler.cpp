#include "glfwHandler.hpp"

GlfwHandler::GlfwHandler()
	:
	WIDTH(800),
	HEIGHT(600)
{

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = std::unique_ptr<GLFWwindow, GLFWwindowDeleter>(
		glfwCreateWindow(WIDTH, HEIGHT, "Vulkan 3D Model Viewer", nullptr, nullptr));
	
}

GlfwHandler::~GlfwHandler()
{
	glfwTerminate();
}
