#pragma once
#include "RendererCommon.h"
#include "DeviceManager.hpp"
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class Texture
{
public:
	Texture() = default;
	~Texture() = default;

	Texture(DeviceManager& deviceManager, const VkCommandPool& commandPool,
	        const std::string& path);

	void cleanup(VkDevice device);

	const VkImageView& getImageView();
	const VkSampler& getSampler();

private:
	void copyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkBuffer buffer, VkImage image,
	                       uint32_t width, uint32_t height);
	void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue queue, VkImage image,
	                           VkFormat format, VkImageLayout oldLayout,
	                           VkImageLayout newLayout, uint32_t mipLevels);
	void generateMipmaps(DeviceManager& deviceManager, VkCommandPool commandPool, VkImage image, VkFormat imageFormat,
	                     int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
	void createTextureSampler(DeviceManager& deviceManager);

	float mipLevels;
	VkImage textureImage;
	VkDeviceMemory textureImaageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
};
