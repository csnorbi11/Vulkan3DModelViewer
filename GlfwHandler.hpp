#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

struct GLFWwindowDeleter {
	void operator()(GLFWwindow* window) {
		if (window) {
			glfwDestroyWindow(window);
		}
	}
};

struct GlfwHandler {
	std::unique_ptr<GLFWwindow, GLFWwindowDeleter> window;
	uint32_t WIDTH, HEIGHT;
	
	GlfwHandler();
	~GlfwHandler();

};