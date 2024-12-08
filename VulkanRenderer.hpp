#include "RendererCommon.h"
#include "ValidationLayers.hpp"

#include <vector>



class VulkanRenderer {
public:
	VulkanRenderer(GLFWwindow* window);
	~VulkanRenderer();

private:
	VkInstance instance;

	ValidationLayers validationLayers;




	

private:
	void createInstance();



};


