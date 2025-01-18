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
	renderer.addLightSource();

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
	ModelPropertiesGUI(renderer.models);
	ImGui::End();
	ImGui::Begin("Light Transformer");
	modelWindowSize = ImVec2(glfwHandler.WIDTH / 4, glfwHandler.HEIGHT / 2);
	ImGui::SetWindowSize(modelWindowSize);
	posOffsetX = glfwHandler.WIDTH - modelWindowSize.x;
	ImGui::SetWindowPos(ImVec2(posOffsetX, glfwHandler.HEIGHT / 2));
	ImGui::Text("Number of light sources: %d", renderer.lightSources.size());
	if (ImGui::Button("Add light"))
	{
		renderer.addLightSource();
	}
	LightPropertiesGUI(renderer.lightSources);
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

void App::ModelPropertiesGUI(std::vector<Model>& models)
{
	int i = 1;
	for (auto& model : models)
	{
		ImGui::BeginChild(i, modelWindowSize, true);
		ImGui::LabelText("Name", model.name.c_str());
		if (ImGui::Button("Delete"))
		{
			renderer.deleteModel(model);
			ImGui::EndChild();
			break;
		}
		if (ImGui::Button("Reset Position"))
		{
			model.position = glm::vec3(0.0f);
		}
		if (ImGui::Button("Reset Rotation"))
		{
			model.rotation = glm::vec3(0.0f);
		}
		if (ImGui::Button("Reset Scale"))
		{
			model.scale = glm::vec3(1.0f);
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

void App::LightPropertiesGUI(std::vector<LightSource>& lightSources)
{
	int i = 1;
	for (auto& light_source : lightSources)
	{
		ImGui::BeginChild(i, modelWindowSize, true);
		ImGui::LabelText("Name", light_source.name.c_str());
		if (ImGui::Button("Delete"))
		{
			renderer.deleteLightSource(light_source);
			ImGui::EndChild();
			break;
		}
		if (ImGui::Button("Reset Position"))
		{
			light_source.position = glm::vec3(0.0f);
		}
		if (ImGui::Button("Reset Color"))
		{
			light_source.color = glm::vec3(1.0f);
		}
		if (ImGui::Button("Reset Intensity"))
		{
			light_source.intensity = 10.0f;
		}
		ImGui::DragFloat("x", &light_source.position.x, 0.1f);
		ImGui::DragFloat("y", &light_source.position.y, 0.1f);
		ImGui::DragFloat("z", &light_source.position.z, 0.1f);
		ImGui::DragFloat("Intensity: ", &light_source.intensity, 0.1f, 0.0f);
		light_source.intensity = std::clamp(light_source.intensity, 0.0f, (float)UINT32_MAX);
		float* lightColor[3];
		lightColor[0] = &light_source.color.x;
		lightColor[1] = &light_source.color.y;
		lightColor[2] = &light_source.color.z;
		ImGui::ColorPicker3("Light Color/Intensity", *lightColor);
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
