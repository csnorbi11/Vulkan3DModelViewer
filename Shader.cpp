#include "Shader.hpp"




void Shader::createModule(VkDevice device, const std::string& filename)
{
	auto code = readFile(filename);

	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}
}

Shader::~Shader()
{
	vkDestroyShaderModule(device, shaderModule, nullptr);
}

Shader::Shader(VkDevice device, const std::string& filename,
	VertexAttribute vertexAttributes)
	:
	device(device)
{
	createModule(device, filename);

	if (vertexAttributes == (VertexAttribute::POSITION | VertexAttribute::COLOR))
	{
		bindingDescription.stride = sizeof(VertexColor);

		attributeDescriptions.resize(2);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexColor, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 3;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexColor, color);
	}
	else if (vertexAttributes == (VertexAttribute::POSITION | VertexAttribute::NORMAL |
		VertexAttribute::TEXCOORD))
	{
		bindingDescription.stride = sizeof(VertexNormalTexture);

		attributeDescriptions.resize(3);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexNormalTexture, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexNormalTexture, normal);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(VertexNormalTexture, texCoord);
	}
	else
	{
		throw std::runtime_error("Invalid vertex attributes");
	}

	bindingDescription.binding = 0;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


}

VkShaderModule Shader::getModule() const
{
	return shaderModule;
}

const VkVertexInputBindingDescription& Shader::getBindingDescription() const
{
	return bindingDescription;
}

const std::vector<VkVertexInputAttributeDescription>& Shader::getAttributeDescriptions() const
{
	return attributeDescriptions;
}

void Shader::cleanup(VkDevice device)
{
	vkDestroyShaderModule(device, shaderModule, nullptr);
}
