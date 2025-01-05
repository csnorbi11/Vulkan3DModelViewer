#pragma once
#include "GlfwHandler.hpp"
#include "VulkanRenderer.hpp"
#include "ModelLoader.hpp"

#include "ImGuiFileDialogConfig.h"
#include "ImGuiFileDialog.h"

static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

class App {
public:
	App();
	
	~App();

	void run();

private:
	void loop();

	void initImGui();
	void ModelLoaderDialog(bool& flipY);
	void ModelPropertiesGUI();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	GlfwHandler glfwHandler;
	VulkanRenderer renderer;
	ModelLoader modelLoader;
	Camera camera;

};