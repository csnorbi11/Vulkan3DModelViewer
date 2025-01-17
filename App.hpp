#pragma once
#include "GlfwHandler.hpp"
#include "VulkanRenderer.hpp"
#include "ModelLoader.hpp"

#include "imguiheaders/ImGuiFileDialogConfig.h"
#include "imguiheaders/ImGuiFileDialog.h"

static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

class App
{
public:
	App();
	~App();

	void run();

private:
	void loop();

	void MenuWindow();
	void ModelHandlerWIndow(bool& flipY);

	void initImGui();
	void ModelLoaderDialog(bool& flipY);
	template <class T>
	void ObjectPropertiesGUI(std::vector<T>& vector);

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	GlfwHandler glfwHandler;
	VulkanRenderer renderer;
	ModelLoader modelLoader;
	Camera camera;

	ImVec2 modelWindowSize;
};
