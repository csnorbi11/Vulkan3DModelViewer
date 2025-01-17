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
	renderer(glfwHandler.window.get(), glfwHandler.WIDTH, glfwHandler.HEIGHT, camera,
		"config.txt"),
	modelLoader(renderer.getDeviceManager(), renderer.getCommandBuffer().getCommandPool()),
	camera(glfwHandler.window.get(), { 0.0, 0.0, -3.0 }, 5.0f)
{
	glfwSetWindowUserPointer(glfwHandler.window.get(), this);
	glfwSetWindowSizeCallback(glfwHandler.window.get(), framebufferResizeCallback);
	glfwSetCursorPosCallback(glfwHandler.window.get(), mouseCallback);
	globalCamera = &camera;

	initImGui();
}

App::~App()
{
	renderer.deleteAllModels();

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


	renderer.recieveModel(modelLoader.loadModel("backpack.obj"));

	while (!glfwWindowShouldClose(glfwHandler.window.get()))
	{
		startTime = glfwGetTime();
		glfwPollEvents();

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
		ModelHandlerWIndow(flipY);

		MenuWindow();

		ImGui::EndFrame();


		camera.update(static_cast<float>(deltaTime));

		renderer.drawFrame();


		endTime = glfwGetTime();
		deltaTime = endTime - startTime;
	}
	renderer.wait();
}

void App::MenuWindow()
{
	ImGui::Begin("Menu");
	ImVec2 menuWindowSize(glfwHandler.WIDTH / 6, glfwHandler.HEIGHT / 2);
	ImGui::SetWindowSize(menuWindowSize);
	ImGui::SetWindowPos(ImVec2(0, 0));

	ImGui::DragFloat("Camera speed: ", &camera.moveSpeed, 0.1f);
	ImGui::End();
}

void App::ModelHandlerWIndow(bool& flipY)
{
	ImGui::Begin("Model Loader/Transformer");
	modelWindowSize = ImVec2(glfwHandler.WIDTH / 4, glfwHandler.HEIGHT / 2);
	ImGui::SetWindowSize(modelWindowSize);
	int posOffsetX = glfwHandler.WIDTH - modelWindowSize.x;
	ImGui::SetWindowPos(ImVec2(posOffsetX, 0));
	ModelLoaderDialog(flipY);
	ImGui::Text("Number of models: %d", renderer.models.size());
	ObjectPropertiesGUI(renderer.models);
	ImGui::End();
	ImGui::Begin("Light Transformer");
	modelWindowSize = ImVec2(glfwHandler.WIDTH / 4, glfwHandler.HEIGHT / 2);
	ImGui::SetWindowSize(modelWindowSize);
	posOffsetX = glfwHandler.WIDTH - modelWindowSize.x;
	ImGui::SetWindowPos(ImVec2(posOffsetX, glfwHandler.HEIGHT / 2));
	ImGui::Text("Number of light sources: %d", renderer.lightSources.size());
	ObjectPropertiesGUI(renderer.lightSources);
	ImGui::End();
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

template <class T>
void App::ObjectPropertiesGUI(std::vector<T>& vector)
{
	int i = 1;
	for (auto& object : vector)
	{
		ImGui::BeginChild(i, modelWindowSize, true);
		ImGui::LabelText("Name", object.name.c_str());
		if (ImGui::Button("Delete"))
		{
			if (typeid(object) == typeid(Model))
				renderer.deleteModel(dynamic_cast<Model&>(object));
			else if (typeid(object) == typeid(LightSource))
				renderer.deleteLightSource(dynamic_cast<LightSource&>(object));
			ImGui::EndChild();
			break;
		}
		if (ImGui::Button("Reset Position"))
		{
			object.position = glm::vec3(0.0f);
		}
		if (ImGui::Button("Reset Rotation"))
		{
			object.rotation = glm::vec3(0.0f);
		}
		if (ImGui::Button("Reset Scale"))
		{
			object.scale = glm::vec3(1.0f);
		}
		ImGui::DragFloat("x", &object.position.x, 0.1f);
		ImGui::DragFloat("y", &object.position.y, 0.1f);
		ImGui::DragFloat("z", &object.position.z, 0.1f);
		ImGui::DragFloat("pitch", &object.rotation.x, 0.1f);
		ImGui::DragFloat("yaw", &object.rotation.y, 0.1f);
		ImGui::DragFloat("roll", &object.rotation.z, 0.1f);
		ImGui::DragFloat("ScaleX: ", &object.scale.x, 0.1f, 0.0f);
		ImGui::DragFloat("ScaleY", &object.scale.y, 0.1f, 0.0f);
		ImGui::DragFloat("ScaleZ", &object.scale.z, 0.1f, 0.0f);
		ImGui::EndChild();
		i++;
	}
}

void App::ModelLoaderDialog(bool& flipY)
{
	if (ImGui::Button("Load Model"))
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".obj", config);
	}
	ImGui::Checkbox("Flip Y", &flipY);
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
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
