#pragma once
#include "RendererCommon.h"
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class Texture {
public:
	Texture() = default;
	~Texture();

	Texture(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
		const VkCommandPool& commandPool, const VkQueue& graphicsQueue,
		const std::string& path,const VkPhysicalDeviceProperties& properties);

	void cleanup();

	const VkImageView& getImageView();
	const VkSampler& getSampler();

private:
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
		VkImageLayout newLayout, uint32_t mipLevels);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels,
		const VkPhysicalDevice& physicalDevice);
	void createTextureSampler();

	std::string PATH;

	float mipLevels;
	VkImage textureImage;
	VkDeviceMemory textureImaageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	VkDevice device;
	VkPhysicalDeviceProperties properties;
	VkQueue queue;
	VkCommandPool commandPool;
};