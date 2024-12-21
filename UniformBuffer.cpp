#include "UniformBuffer.hpp"


UniformBuffer::UniformBuffer()
{
}

UniformBuffer::~UniformBuffer()
{
}

UniformBuffer::UniformBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
	const int MAX_FRAMES_IN_FLIGHT, const VkExtent2D& swapchainExtent, VkPhysicalDeviceProperties properties)
	:
	device(device),
	swapchainExtent(swapchainExtent),
	staticUbo{}
{
	size_t minUboAlignment = properties.limits.minUniformBufferOffsetAlignment;
	dynamicAlignment = sizeof(glm::mat4);
	if (minUboAlignment > 0)
		dynamicAlignment = (sizeof(glm::mat4) + minUboAlignment - 1) & ~(minUboAlignment - 1);

	bufferSize = dynamicAlignment * MAX_FRAMES_IN_FLIGHT * 1;


	create(MAX_FRAMES_IN_FLIGHT, physicalDevice);
	createDescriptorSetLayout();
	createDescriptorPool(MAX_FRAMES_IN_FLIGHT);
	createDescriptorSets(MAX_FRAMES_IN_FLIGHT);
}

void UniformBuffer::create(const int MAX_FRAMES_IN_FLIGHT, const VkPhysicalDevice& physicalDevice)
{
	uniformBuffers.staticBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffers.dynamicBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	uniformBuffers.dynamicBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffers.dynamicBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	uniformBuffers.staticBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffers.staticBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(sizeof(StaticUbo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers.staticBuffers[i], uniformBuffers.staticBuffersMemory[i], device, physicalDevice);
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers.dynamicBuffers[i], uniformBuffers.dynamicBuffersMemory[i], device, physicalDevice);

		vkMapMemory(device, uniformBuffers.staticBuffersMemory[i], 0, sizeof(StaticUbo), 0, &uniformBuffers.staticBuffersMapped[i]);
		vkMapMemory(device, uniformBuffers.dynamicBuffersMemory[i], 0, bufferSize, 0, &uniformBuffers.dynamicBuffersMapped[i]);

	}
}

void UniformBuffer::updateStatic(uint32_t currentFrame)
{

	staticUbo.view = glm::lookAt(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	staticUbo.proj = glm::perspective(glm::radians(80.0f), swapchainExtent.width / (float)swapchainExtent.height, 0.1f, 100.0f);
	staticUbo.proj[1][1] *= -1;

	memcpy(uniformBuffers.staticBuffersMapped[currentFrame], &staticUbo, sizeof(staticUbo));
}
void UniformBuffer::updateDynamic(uint32_t currentFrame)
{

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


	dynamicUbo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	dynamicUbo.model = glm::rotate(dynamicUbo.model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	dynamicUbo.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	


	memcpy(uniformBuffers.dynamicBuffersMapped[currentFrame], &dynamicUbo.model, bufferSize);
	VkMappedMemoryRange memoryRange{};
	memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	memoryRange.memory = uniformBuffers.dynamicBuffersMemory[currentFrame];
	memoryRange.offset = 0;
	memoryRange.size = bufferSize;
	vkFlushMappedMemoryRanges(device, 1, &memoryRange);
}
void UniformBuffer::cleanup()
{
	for (size_t i = 0; i < uniformBuffers.dynamicBuffers.size(); i++) {
		vkDestroyBuffer(device, uniformBuffers.dynamicBuffers[i], nullptr);
		vkDestroyBuffer(device, uniformBuffers.staticBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffers.dynamicBuffersMemory[i], nullptr);
		vkFreeMemory(device, uniformBuffers.staticBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}
const VkDescriptorSetLayout& UniformBuffer::getLayout()
{
	return descriptorSetLayout;
}
const std::vector<VkDescriptorSet>& UniformBuffer::getSets()
{
	return descriptorSets;
}
uint32_t UniformBuffer::getDynamicAlignment()
{
	return dynamicAlignment;
}
void UniformBuffer::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	//VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	//samplerLayoutBinding.binding = 1;
	//samplerLayoutBinding.descriptorCount = 1;
	//samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//samplerLayoutBinding.pImmutableSamplers = nullptr;
	//samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding dynamicboLayoutBinding{};
	dynamicboLayoutBinding.binding = 1;
	dynamicboLayoutBinding.descriptorCount = 1;
	dynamicboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	dynamicboLayoutBinding.pImmutableSamplers = nullptr;
	dynamicboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, dynamicboLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	
}
void UniformBuffer::createDescriptorPool(const int MAX_FRAMES_IN_FLIGHT)
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	//poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}
void UniformBuffer::createDescriptorSets(const int MAX_FRAMES_IN_FLIGHT)
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT * 2);
	for (size_t i = 0; i < layouts.size()/2; i++) {
		layouts[i] = descriptorSetLayout;
		layouts[i + MAX_FRAMES_IN_FLIGHT] = descriptorSetLayout;
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	dynamicDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo staticBufferInfo{};
		staticBufferInfo.buffer = uniformBuffers.staticBuffers[i];
		staticBufferInfo.offset = 0;
		staticBufferInfo.range = sizeof(staticUbo);

		VkDescriptorBufferInfo dynamicBufferInfo{};
		dynamicBufferInfo.buffer = uniformBuffers.dynamicBuffers[i];
		dynamicBufferInfo.offset = 0;
		dynamicBufferInfo.range = sizeof(dynamicUbo);

		std::array<VkWriteDescriptorSet, 1> descriptorWrites{};
		VkWriteDescriptorSet dynamicDescriptorWrite{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &staticBufferInfo;
		descriptorWrites[0].pImageInfo = nullptr;
		descriptorWrites[0].pTexelBufferView = nullptr;

		dynamicDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		dynamicDescriptorWrite.dstSet = descriptorSets[i];
		dynamicDescriptorWrite.dstBinding = 1;
		dynamicDescriptorWrite.dstArrayElement = 0;
		dynamicDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		dynamicDescriptorWrite.descriptorCount = 1;
		dynamicDescriptorWrite.pBufferInfo = &dynamicBufferInfo;
		dynamicDescriptorWrite.pImageInfo = nullptr;
		dynamicDescriptorWrite.pTexelBufferView = nullptr;

		//descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//descriptorWrites[1].dstSet = descriptorSets[i];
		//descriptorWrites[1].dstBinding = 1;
		//descriptorWrites[1].dstArrayElement = 0;
		//descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//descriptorWrites[1].descriptorCount = 1;
		//descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, 1, &descriptorWrites[0], 0, nullptr);
		vkUpdateDescriptorSets(device, 1, &dynamicDescriptorWrite, 0, nullptr);
		//vkUpdateDescriptorSets(device, 1, &dynamicDescriptorWrites[0], 0, nullptr);
	}
}