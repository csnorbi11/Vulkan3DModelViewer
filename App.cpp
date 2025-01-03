#include "App.hpp"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

App::App()
	:
	glfwHandler(),
	camera(glfwHandler.window.get()),
	renderer(glfwHandler.window.get(),camera),
	modelLoader(renderer.getDeviceManager().getDevice(),
		renderer.getDeviceManager().getPhysicalDevice(),
		renderer.getCommandBuffer().getCommandPool(), 
		renderer.getDeviceManager().getGraphicsQueue(),
		renderer.getDeviceManager().getPhysicalDeviceProperties())
{
	renderer.recieveModel(modelLoader.loadModel("backpack.obj"));
	
	glfwSetWindowUserPointer(glfwHandler.window.get(), this);
	glfwSetWindowSizeCallback(glfwHandler.window.get(), framebufferResizeCallback);


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplGlfw_InitForVulkan(glfwHandler.window.get(), true);
	ImGui_ImplVulkan_InitInfo initInfo = {};
	initInfo.Instance = renderer.getInstance();
	initInfo.PhysicalDevice = renderer.getDeviceManager().getPhysicalDevice();
	initInfo.Device = renderer.getDeviceManager().getDevice();
	initInfo.QueueFamily = renderer.getDeviceManager().getIndices().graphicsFamily.value();
	initInfo.Queue = renderer.getDeviceManager().getGraphicsQueue();
	initInfo.PipelineCache = VK_NULL_HANDLE;
	initInfo.DescriptorPool = renderer.getUniformBuffer().getDescriptorPool();
	initInfo.Allocator = nullptr;
	initInfo.MinImageCount = 2;
	initInfo.ImageCount = 2;
	initInfo.MSAASamples = renderer.getSwapchainManager().getMsaa().getSampleCount();
	initInfo.RenderPass = renderer.getSwapchainManager().getRenderPass().getRenderPass();
	initInfo.CheckVkResultFn = nullptr;
	ImGui_ImplVulkan_Init(&initInfo);
	ImGui_ImplVulkan_CreateFontsTexture();

}

App::~App()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
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

	double startTime = 0;
	double endTime = 0;
	double deltaTime = 0;

	while (!glfwWindowShouldClose(glfwHandler.window.get())) {
		startTime = glfwGetTime();
		glfwPollEvents();
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		;
		;
		if (ImGui::Button("Open File Dialog")) {
			IGFD::FileDialogConfig config;
			config.path = ".";
			ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".obj", config);
		}
		// display
		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
			if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				// action
				renderer.recieveModel(modelLoader.loadModel(filePathName));
			}

			// close
			ImGuiFileDialog::Instance()->Close();
		}

		
		camera.update(deltaTime);
		
		renderer.drawFrame();


		endTime = glfwGetTime();
		deltaTime = endTime - startTime;
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


