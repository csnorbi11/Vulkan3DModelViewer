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
	staticUbo.camPos = camera.getPosition();
	staticUbo.view = camera.getViewMatrix();
	staticUbo.proj = glm::perspective(glm::radians(80.0f),
		swapchainExtent.width / static_cast<float>(swapchainExtent.height), 0.1f, 100.0f);
	staticUbo.proj[1][1] *= -1;

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
	for (size_t i=0;i<lightSources.size();i++)
	{
		auto modelMat = (glm::mat4*)(((uint64_t)dynamicUbo.model + (offset * dynamicAlignment)));
		offset++;
		*modelMat = translate(glm::mat4(1.0), lightSources[i].position);
		*modelMat = rotate(*modelMat, glm::radians(lightSources[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		*modelMat = rotate(*modelMat, glm::radians(lightSources[i].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		*modelMat = rotate(*modelMat, glm::radians(lightSources[i].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		*modelMat = scale(*modelMat, lightSources[i].scale);
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
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding dynamicboLayoutBinding{};
	dynamicboLayoutBinding.binding = 1;
	dynamicboLayoutBinding.descriptorCount = 1;
	dynamicboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	dynamicboLayoutBinding.pImmutableSamplers = nullptr;
	dynamicboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 2;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 3> bindings = {
		uboLayoutBinding, dynamicboLayoutBinding, samplerLayoutBinding
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

void UniformBuffer::createDescriptorSets(Object& object, const int MAX_FRAMES_IN_FLIGHT)
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT * 3);
	layouts[0] = descriptorSetLayout;
	layouts[1] = descriptorSetLayout;
	layouts[2] = descriptorSetLayout;

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	object.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device, &allocInfo, object.descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo staticBufferInfo{};
		staticBufferInfo.buffer = uniformBuffers.staticBuffers[i];
		staticBufferInfo.offset = 0;
		staticBufferInfo.range = sizeof(staticUbo);

		VkDescriptorBufferInfo modelDynamicBufferInfo{};
		modelDynamicBufferInfo.buffer = uniformBuffers.dynamicBuffers[i];
		modelDynamicBufferInfo.offset = 0;
		modelDynamicBufferInfo.range = dynamicAlignment;

		VkDescriptorBufferInfo lightSourcesDynamicBufferInfo{};

		VkWriteDescriptorSet staticDescriptorWrite{};
		VkWriteDescriptorSet modelDynamicDescriptorWrite{};
		VkWriteDescriptorSet lightSourcesDynamicDescriptorWrite{};

		staticDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		staticDescriptorWrite.dstSet = object.descriptorSets[i];
		staticDescriptorWrite.dstBinding = 0;
		staticDescriptorWrite.dstArrayElement = 0;
		staticDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		staticDescriptorWrite.descriptorCount = 1;
		staticDescriptorWrite.pBufferInfo = &staticBufferInfo;
		staticDescriptorWrite.pImageInfo = nullptr;
		staticDescriptorWrite.pTexelBufferView = nullptr;

		modelDynamicDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		modelDynamicDescriptorWrite.dstSet = object.descriptorSets[i];
		modelDynamicDescriptorWrite.dstBinding = 1;
		modelDynamicDescriptorWrite.dstArrayElement = 0;
		modelDynamicDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		modelDynamicDescriptorWrite.descriptorCount = 1;
		modelDynamicDescriptorWrite.pBufferInfo = &modelDynamicBufferInfo;
		modelDynamicDescriptorWrite.pImageInfo = nullptr;
		modelDynamicDescriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(device, 1, &staticDescriptorWrite, 0, nullptr);
		vkUpdateDescriptorSets(device, 1, &modelDynamicDescriptorWrite, 0, nullptr);

		if (typeid(object).name() != typeid(Model).name())
		{
			continue;
		}
		auto model = dynamic_cast<Model*>(&object);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = model->getTextures()[0].getImageView();
		imageInfo.sampler = model->getTextures()[0].getSampler();

		VkWriteDescriptorSet textureDescriptorWrite{};
		textureDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		textureDescriptorWrite.dstSet = model->descriptorSets[i];
		textureDescriptorWrite.dstBinding = 2;
		textureDescriptorWrite.dstArrayElement = 0;
		textureDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		textureDescriptorWrite.descriptorCount = 1;
		textureDescriptorWrite.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, 1, &textureDescriptorWrite, 0, nullptr);
	}
}
