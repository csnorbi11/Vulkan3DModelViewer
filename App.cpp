#include "App.hpp"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>


double lastX;
double lastY;

double xOffset;
double yOffset;

Camera* globalCamera;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (globalCamera == nullptr)
		return;

	double xposCurrent = xpos;
	double yposCurrent = ypos;

	xOffset = xposCurrent - lastX;
	yOffset = lastY - yposCurrent;

	lastX = xposCurrent;
	lastY = yposCurrent;

	globalCamera->processMouseInput(xOffset, yOffset);
}

App::App()
	:
	glfwHandler(),
	camera(glfwHandler.window.get(), { 0.0,0.0,-3.0 }),
	renderer(glfwHandler.window.get(), camera),
	modelLoader(renderer.getDeviceManager().getDevice(),
		renderer.getDeviceManager().getPhysicalDevice(),
		renderer.getCommandBuffer().getCommandPool(),
		renderer.getDeviceManager().getGraphicsQueue(),
		renderer.getDeviceManager().getPhysicalDeviceProperties())
{

	glfwSetWindowUserPointer(glfwHandler.window.get(), this);
	glfwSetWindowSizeCallback(glfwHandler.window.get(), framebufferResizeCallback);
	glfwSetCursorPosCallback(glfwHandler.window.get(), mouseCallback);
	globalCamera = &camera;

	initImGui();

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

	double startTime = 0;
	double endTime = 0;
	double deltaTime = 0;
	bool flipY = false;
	while (!glfwWindowShouldClose(glfwHandler.window.get())) {
		startTime = glfwGetTime();
		glfwPollEvents();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(300, 100));
		ImGui::NewFrame();
		ModelLoaderDialog(flipY);
		ModelPropertiesGUI();





		camera.update(static_cast<float>(deltaTime));

		renderer.drawFrame();


		endTime = glfwGetTime();
		deltaTime = endTime - startTime;
	}
	renderer.wait();

}

void App::initImGui()
{
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
void App::ModelPropertiesGUI()
{
	int i = 1;
	for (auto& model : renderer.models) {
		ImGui::BeginChild(i, ImVec2(300, 300), true);
		ImGui::LabelText("Name", model.name.c_str());
		if (ImGui::Button("Delete")) {
			renderer.models.erase(renderer.models.begin() + i - 1);
			ImGui::EndChild();
			break;
		}
		if (ImGui::Button("Reset Position")) {
			model.position = glm::vec3(0.0f);
		}
		if (ImGui::Button("Reset Rotation")) {
			model.rotation = glm::vec3(0.0f);
		}
		ImGui::DragFloat("x", &model.position.x, 0.1f);
		ImGui::DragFloat("y", &model.position.y, 0.1f);
		ImGui::DragFloat("z", &model.position.z, 0.1f);
		ImGui::DragFloat("pitch", &model.rotation.x, 0.1f);
		ImGui::DragFloat("yaw", &model.rotation.y, 0.1f);
		ImGui::DragFloat("roll", &model.rotation.z, 0.1f);
		ImGui::DragFloat("ScaleX: ", &model.scale.x, 0.1f, 0.0f);
		ImGui::DragFloat("ScaleY", &model.scale.y, 0.1f, 0.0f);
		ImGui::DragFloat("ScaleZ", &model.scale.z, 0.1f, 0.0f);
		ImGui::EndChild();
		i++;
	}
}
void App::ModelLoaderDialog(bool& flipY)
{
	if (ImGui::Button("Load Model")) {
		IGFD::FileDialogConfig config;
		config.path = ".";
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".obj", config);
	}
	ImGui::Checkbox("Flip Y", &flipY);
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {


		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			renderer.recieveModel(modelLoader.loadModel(filePathName, flipY));
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void App::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
	app->renderer.framebufferResized = true;
}


