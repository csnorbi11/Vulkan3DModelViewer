#include "RendererCommon.h"

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	{
		QueueFamilyIndices indices;

		uint32_t familyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilies.data());

		int i = 0;
		for (const auto& family : queueFamilies)
		{
			if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupported = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupported);
			if (presentSupported)
			{
				indices.presentFamily = i;
			}

			if (indices.isComplete())
				break;
			i++;
		}

		return indices;
	}
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presents.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presents.data());
	}

	return details;
}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type");
}

void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format,
                 VkSampleCountFlagBits numberOfSamples, VkImageTiling tiling, VkImageUsageFlags usage,
                 VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory,
                 const VkDevice& device, const VkPhysicalDevice& physicalDevice)
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = numberOfSamples;
	imageInfo.flags = 0;

	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
	{
		throw std::runtime_error("cannot create texture image");
	}


	VkMemoryRequirements memRequirments;
	vkGetImageMemoryRequirements(device, image, &memRequirments);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirments.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirments.memoryTypeBits, properties, physicalDevice);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to alloc image memory");
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                            uint32_t mipLevels, VkDevice device)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file: " + filename);
	}

	size_t fileSize = file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                  VkBuffer& buffer, VkDeviceMemory& bufferMemory, const VkDevice& device,
                  const VkPhysicalDevice& physicalDevice)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer");
	}

	VkMemoryRequirements memRequirments{};
	vkGetBufferMemoryRequirements(device, buffer, &memRequirments);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirments.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirments.memoryTypeBits, properties, physicalDevice);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != EXIT_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size,
                const VkCommandPool& commandPool, const VkDevice& device, const VkQueue& queue)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool, device);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer, queue, commandPool, device);
}

VkCommandBuffer beginSingleTimeCommands(const VkCommandPool& commandPool, const VkDevice& device)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void endSingleTimeCommands(VkCommandBuffer commandBuffer, const VkQueue& queue,
                           const VkCommandPool& commandPool, const VkDevice& device)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
