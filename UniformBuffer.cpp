#include "UniformBuffer.hpp"

void* alignedAlloc(size_t size, size_t alignment)
{
	void* data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
	data = _aligned_malloc(size, alignment);
#else
	int res = posix_memalign(&data, alignment, size);
	if (res != 0)
		data = nullptr;
#endif
	return data;
}

UniformBuffer::~UniformBuffer()
{
}

UniformBuffer::UniformBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice,
	const int MAX_FRAMES_IN_FLIGHT, const VkExtent2D& swapchainExtent,
	VkPhysicalDeviceProperties properties,
	const std::vector<Model>& models, const std::vector<LightSource>& lightSources,
	const Camera& camera)
	:
	models(models),
	lightSources(lightSources),
	camera(camera),
	MAX_MODEL_COUNT(1000),
	staticUbo{},
	device(device),
	physicalDevice(physicalDevice),
	swapchainExtent(swapchainExtent),
	MAX_FRAMES_IN_FLIGHT(MAX_FRAMES_IN_FLIGHT)
{
	size_t minUboAlignment = properties.limits.minUniformBufferOffsetAlignment;
	dynamicAlignment = sizeof(glm::mat4);
	if (minUboAlignment > 0)
		dynamicAlignment = (sizeof(glm::mat4) + minUboAlignment - 1) & ~(minUboAlignment - 1);
	bufferSize = 0;
	dynamicUbo.model = static_cast<glm::mat4*>(alignedAlloc(bufferSize, dynamicAlignment));

	create(MAX_FRAMES_IN_FLIGHT, physicalDevice);
	createDescriptorSetLayout();
	createDescriptorPool(MAX_FRAMES_IN_FLIGHT);

}

void UniformBuffer::create(const int MAX_FRAMES_IN_FLIGHT, const VkPhysicalDevice& physicalDevice)
{
	uniformBuffers.staticBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffers.dynamicBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	uniformBuffers.dynamicBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffers.dynamicBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	uniformBuffers.staticBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffers.staticBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		createBuffer(sizeof(StaticUbo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers.staticBuffers[i], uniformBuffers.staticBuffersMemory[i], device, physicalDevice);
		vkMapMemory(device, uniformBuffers.staticBuffersMemory[i], 0, sizeof(StaticUbo), 0,
			&uniformBuffers.staticBuffersMapped[i]);
	}
}

void UniformBuffer::recreateDynamicBuffer()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyBuffer(device, uniformBuffers.dynamicBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffers.dynamicBuffersMemory[i], nullptr);
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers.dynamicBuffers[i], uniformBuffers.dynamicBuffersMemory[i], device, physicalDevice);
		vkMapMemory(device, uniformBuffers.dynamicBuffersMemory[i], 0, bufferSize, 0,
			&uniformBuffers.dynamicBuffersMapped[i]);
	}
}

void UniformBuffer::calculateDynamicBuffer()
{
	bufferSize = dynamicAlignment * MAX_FRAMES_IN_FLIGHT * (models.size() + lightSources.size());

	dynamicUbo.model = static_cast<glm::mat4*>(_aligned_realloc(dynamicUbo.model, bufferSize, dynamicAlignment));
	assert(dynamicUbo.model);
}

void UniformBuffer::updateStatic(uint32_t currentFrame)
{
	staticUbo.camPos.x = camera.getPosition().x;
	staticUbo.camPos.y = camera.getPosition().y;
	staticUbo.camPos.z = camera.getPosition().z;
	staticUbo.view = camera.getViewMatrix();
	staticUbo.proj = glm::perspective(glm::radians(80.0f),
		swapchainExtent.width / static_cast<float>(swapchainExtent.height), 0.1f, 100.0f);
	staticUbo.proj[1][1] *= -1;
	for (size_t i = 0; i < std::min(MAX_LIGHTS, lightSources.size()); i++)
	{
		staticUbo.lightSources[i].position = lightSources[i].position;
		staticUbo.lightSources[i].color = lightSources[i].color;
		staticUbo.lightSources[i].intensity = lightSources[i].intensity;
	}

	memcpy(uniformBuffers.staticBuffersMapped[currentFrame], &staticUbo, sizeof(staticUbo));
}

void UniformBuffer::updateDynamic(uint32_t currentFrame)
{
	if (models.empty() && lightSources.empty())
		return;
	uint32_t dim = static_cast<uint32_t>(pow(2, (1.0f / 3.0f)));
	size_t offset = 0;
	for (size_t i = 0; i < models.size(); i++)
	{
		auto modelMat = (glm::mat4*)(((uint64_t)dynamicUbo.model + (offset * dynamicAlignment)));
		offset++;
		*modelMat = translate(glm::mat4(1.0), models[i].position);
		*modelMat = rotate(*modelMat, glm::radians(models[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		*modelMat = rotate(*modelMat, glm::radians(models[i].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		*modelMat = rotate(*modelMat, glm::radians(models[i].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		*modelMat = scale(*modelMat, models[i].scale);
	}
	for (size_t i = 0; i < lightSources.size(); i++)
	{
		auto modelMat = (glm::mat4*)(((uint64_t)dynamicUbo.model + (offset * dynamicAlignment)));
		offset++;
		*modelMat = translate(glm::mat4(1.0), lightSources[i].position);
	}


	memcpy(uniformBuffers.dynamicBuffersMapped[currentFrame], dynamicUbo.model, bufferSize);
	VkMappedMemoryRange memoryRange{};
	memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	memoryRange.memory = uniformBuffers.dynamicBuffersMemory[currentFrame];
	memoryRange.offset = 0;
	memoryRange.size = bufferSize;
	vkFlushMappedMemoryRanges(device, 1, &memoryRange);
}

void UniformBuffer::cleanup()
{
	for (size_t i = 0; i < uniformBuffers.dynamicBuffers.size(); i++)
	{
		vkDestroyBuffer(device, uniformBuffers.dynamicBuffers[i], nullptr);
		vkDestroyBuffer(device, uniformBuffers.staticBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffers.dynamicBuffersMemory[i], nullptr);
		vkFreeMemory(device, uniformBuffers.staticBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}

VkDescriptorSetLayout& UniformBuffer::getLayout()
{
	return descriptorSetLayout;
}

uint32_t UniformBuffer::getDynamicAlignment()
{
	return dynamicAlignment;
}

VkDescriptorPool& UniformBuffer::getDescriptorPool()
{
	return descriptorPool;
}

void UniformBuffer::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding dynamicboLayoutBinding{};
	dynamicboLayoutBinding.binding = 1;
	dynamicboLayoutBinding.descriptorCount = 1;
	dynamicboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	dynamicboLayoutBinding.pImmutableSamplers = nullptr;
	dynamicboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding1{};
	samplerLayoutBinding1.binding = 2;
	samplerLayoutBinding1.descriptorCount = 1;
	samplerLayoutBinding1.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding1.pImmutableSamplers = nullptr;
	samplerLayoutBinding1.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding2{};
	samplerLayoutBinding2.binding = 3;
	samplerLayoutBinding2.descriptorCount = 1;
	samplerLayoutBinding2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding2.pImmutableSamplers = nullptr;
	samplerLayoutBinding2.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 4> bindings = {
		uboLayoutBinding, dynamicboLayoutBinding, samplerLayoutBinding1, samplerLayoutBinding2
	};
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void UniformBuffer::createDescriptorPool(const int MAX_FRAMES_IN_FLIGHT)
{
	std::array<VkDescriptorPoolSize, 3> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = UINT32_MAX;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	poolSizes[1].descriptorCount = UINT32_MAX;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[2].descriptorCount = UINT32_MAX;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = MAX_MODEL_COUNT;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}
