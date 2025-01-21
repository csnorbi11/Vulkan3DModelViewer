#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <stdexcept>
#include <algorithm>

#include <fstream>
#include <iostream>

#include <string>
#include <optional>

#include <vector>
#include <set>
#include <array>

#if NDEBUG
const bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

enum class VertexAttribute : unsigned int
{
	POSITION = 0x01,
	NORMAL = 0x02,
	TEXCOORD = 0x04,
	COLOR = 0x08,
};

struct VertexColor
{
	glm::vec3 pos;
	glm::vec3 color;
	bool operator==(const VertexColor& other) const
	{
		return pos == other.pos && color == other.color;
	}
};
struct VertexNormalTexture
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	bool operator==(const VertexNormalTexture& other) const
	{
		return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
	}
};

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presents;
};

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);

void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format,
	VkSampleCountFlagBits numberOfSamples, VkImageTiling tiling, VkImageUsageFlags usage,
	VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory,
	const VkDevice& device, const VkPhysicalDevice& physicalDevice);

VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
	uint32_t mipLevels, VkDevice device);

std::vector<char> readFile(const std::string& filename);

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
	VkBuffer& buffer, VkDeviceMemory& bufferMemory, const VkDevice& device,
	const VkPhysicalDevice& physicalDevice);
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
	const VkCommandPool& commandPool, const VkDevice& device, const VkQueue& queue);

VkCommandBuffer beginSingleTimeCommands(const VkCommandPool& commandPool, const VkDevice& device);
void endSingleTimeCommands(VkCommandBuffer commandBuffer, const VkQueue& queue,
	const VkCommandPool& commandPool, const VkDevice& device);
