#include "RendererCommon.h"



class Swapchain {
public:
	Swapchain();
	~Swapchain();

	VkSwapchainKHR get();
	void createSwapChain(VkPhysicalDevice phyDevice, VkDevice device, VkSurfaceKHR surface,
		uint32_t frameBufferWidth, uint32_t frameBufferHeight);
	void recreateSwapChain(VkPhysicalDevice phyDevice, VkDevice device, VkSurfaceKHR surface,
		uint32_t frameBufferWidth, uint32_t frameBufferHeight);

private:
	void cleanupSwapChain();
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaiableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& avaiablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	

	VkSwapchainKHR swapChain;
	uint32_t frameBufferWidth, frameBufferHeight;
};